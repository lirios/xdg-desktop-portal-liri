// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTimer>

#include "logging_p.h"
#include "screencaststream.h"

#include <glib-object.h>

#include <libdrm/drm_fourcc.h>

#include <spa/utils/result.h>

#include <math.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

static enum spa_video_format drmFormatToSpaFormat(uint32_t drmFormat)
{
    switch (drmFormat) {
    case DRM_FORMAT_NV12:
        return SPA_VIDEO_FORMAT_NV12;
    case DRM_FORMAT_ARGB8888:
        return SPA_VIDEO_FORMAT_BGRA;
    case DRM_FORMAT_XRGB8888:
        return SPA_VIDEO_FORMAT_BGRx;
    case DRM_FORMAT_ABGR8888:
        return SPA_VIDEO_FORMAT_RGBA;
    case DRM_FORMAT_XBGR8888:
        return SPA_VIDEO_FORMAT_RGBx;
    case DRM_FORMAT_RGBA8888:
        return SPA_VIDEO_FORMAT_ABGR;
    case DRM_FORMAT_RGBX8888:
        return SPA_VIDEO_FORMAT_xBGR;
    case DRM_FORMAT_BGRA8888:
        return SPA_VIDEO_FORMAT_ARGB;
    case DRM_FORMAT_BGRX8888:
        return SPA_VIDEO_FORMAT_xRGB;
    default:
        return SPA_VIDEO_FORMAT_UNKNOWN;
    };
}

static void onPwCoreError(void *data, uint32_t id, int seq, int res, const char *message)
{
    Q_UNUSED(seq)

    ScreenCastStream *stream = static_cast<ScreenCastStream *>(data);

    qCWarning(lcScreenCast, "Remote error on id:%u: %s", id, message);

    if (id == PW_ID_CORE && res == -EPIPE) {
        stream->freeFrame();
        stream->isStreaming = false;
        Q_EMIT stream->streamStopped();
    }
}

static void onStreamStateChanged(void *data, pw_stream_state old, pw_stream_state state, const char *error_message)
{
    ScreenCastStream *stream = static_cast<ScreenCastStream *>(data);

    qCDebug(lcScreenCast, "Stream state changed from %s to %s",
            pw_stream_state_as_string(old),
            pw_stream_state_as_string(state));

    switch (state) {
    case PW_STREAM_STATE_ERROR:
        qCWarning(lcScreenCast) << "Stream error:" << error_message;
        stream->isStreaming = false;
        Q_EMIT stream->streamStopped();
        break;
    case PW_STREAM_STATE_UNCONNECTED:
    case PW_STREAM_STATE_CONNECTING:
        break;
    case PW_STREAM_STATE_PAUSED:
        if (stream->nodeId() == SPA_ID_INVALID && stream->pwStream) {
            stream->setNodeId(pw_stream_get_node_id(stream->pwStream));
            Q_EMIT stream->streamReady(stream->nodeId());
        }

        if (stream->isStreaming) {
            stream->isStreaming = false;
            Q_EMIT stream->streamStopped();
        }
        break;
    case PW_STREAM_STATE_STREAMING:
        stream->isStreaming = true;
        Q_EMIT stream->streamStarted();
        break;
    }
}

static void onStreamParamChanged(void *data, uint32_t id, const struct spa_pod *param)
{
    ScreenCastStream *stream = static_cast<ScreenCastStream *>(data);

    if (param == nullptr || id  != SPA_PARAM_Format)
        return;

    spa_format_video_raw_parse(param, &stream->pwInfo);

    quint8 buffer[1024];
    spa_pod_builder podBuilder = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

    auto size = stream->frameSet->objects[0]->size;
    auto stride = stream->frameSet->objects[0]->stride;

    const spa_pod *params[2];
    params[0] = static_cast<spa_pod *>(
                spa_pod_builder_add_object(
                    &podBuilder,
                    SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
                    SPA_PARAM_BUFFERS_buffers, SPA_POD_CHOICE_RANGE_Int(1, 1, 32),
                    SPA_PARAM_BUFFERS_blocks, SPA_POD_Int(1),
                    SPA_PARAM_BUFFERS_size, SPA_POD_Int(size),
                    SPA_PARAM_BUFFERS_stride, SPA_POD_Int(stride),
                    SPA_PARAM_BUFFERS_align, SPA_POD_Int(16))
                );
    params[1] = static_cast<spa_pod *>(
                spa_pod_builder_add_object(
                    &podBuilder,
                    SPA_TYPE_OBJECT_ParamMeta, SPA_PARAM_Meta,
                    SPA_PARAM_META_type, SPA_POD_Id(SPA_META_Header),
                    SPA_PARAM_META_size, SPA_POD_Int(sizeof(struct spa_meta_header)))
                );

    pw_stream_update_params(stream->pwStream, params, G_N_ELEMENTS(params));
}

static void onStreamAddBuffer(void *data, struct pw_buffer *buffer)
{
    ScreenCastStream *stream = static_cast<ScreenCastStream *>(data);

    struct spa_data *d = buffer->buffer->datas;
    d[0].type = SPA_DATA_DmaBuf;
    d[0].flags = SPA_DATA_FLAG_READABLE;
    d[0].fd = stream->frameSet->objects[0]->fd;
    d[0].mapoffset = 0;
    d[0].maxsize = stream->frameSet->objects[0]->size;
}

static void onStreamRemoveBuffer(void *data, struct pw_buffer *buffer)
{
    Q_UNUSED(buffer)

    ScreenCastStream *stream = static_cast<ScreenCastStream *>(data);
    stream->freeFrame();
}

static const struct pw_core_events pwCoreEvents = {
    .version = PW_VERSION_CORE_EVENTS,
    .error = onPwCoreError,
};

static const struct pw_stream_events pwStreamEvents = {
    .version = PW_VERSION_STREAM_EVENTS,
    .state_changed = onStreamStateChanged,
    .param_changed = onStreamParamChanged,
    .add_buffer = onStreamAddBuffer,
    .remove_buffer = onStreamRemoveBuffer,
};

// FrameSet

FrameSet::~FrameSet()
{
    reset();
}

void FrameSet::reset()
{
    for (auto *frameObject : objects)
        close(frameObject->fd);
    qDeleteAll(objects);

    ready = false;
}

// ScreenCastStream

ScreenCastStream::ScreenCastStream(WlrExportDmabufFrameV1 *frame, QObject *parent)
    : QObject(parent)
    , m_frame(frame)
    , frameSet(new FrameSet)
{
    connect(frame, &WlrExportDmabufFrameV1::frame, this, &ScreenCastStream::handleFrame);
    connect(frame, &WlrExportDmabufFrameV1::object, this, &ScreenCastStream::handleObject);
    connect(frame, &WlrExportDmabufFrameV1::ready, this, &ScreenCastStream::handleReady);
    connect(frame, &WlrExportDmabufFrameV1::cancel, this, &ScreenCastStream::handleCancel);
}

ScreenCastStream::~ScreenCastStream()
{
    removeStream();

    delete frameSet;
    m_frame->deleteLater();
}

quint32 ScreenCastStream::nodeId() const
{
    return m_nodeId;
}

void ScreenCastStream::setNodeId(quint32 nodeId)
{
    if (m_nodeId == nodeId)
        return;

    m_nodeId = nodeId;
    Q_EMIT nodeIdChanged();
}

bool ScreenCastStream::createStream()
{
    if (!frameSet->ready) {
        qCWarning(lcScreenCast, "Streaming is not ready");
        return false;
    }

    isStreaming = false;

    ///

    pw_init(nullptr, nullptr);

    pwMainLoop = pw_thread_loop_new("screen-cast-portal", nullptr);
    if (!pwMainLoop) {
        qCWarning(lcScreenCast, "Couldn't get PipeWire loop");
        return false;
    }

    pwLoop = pw_thread_loop_get_loop(pwMainLoop);

    pwContext = pw_context_new(pwLoop, nullptr, 0);
    if (!pwContext) {
        qCWarning(lcScreenCast, "Failed to create PipeWire context");
        return false;
    }

    pwCore = pw_context_connect(pwContext, nullptr, 0);
    if (!pwCore) {
        qCWarning(lcScreenCast, "Couldn't connect PipeWire context");
        return false;
    }

    pw_core_add_listener(pwCore, &pwCoreListener, &pwCoreEvents, this);

    ///

    pwStream = pw_stream_new(pwCore, "liri-screen-cast-src", nullptr);
    if (!pwStream) {
        qCWarning(lcScreenCast, "Failed to create PipeWire stream: %s",
                  strerror(errno));
        return false;
    }

    auto resolution =
            SPA_RECTANGLE(
                qBound<uint32_t>(1, frameSet->size.width(), 8192),
                qBound<uint32_t>(1, frameSet->size.height(), 8192)
                );
    auto minResolution = SPA_RECTANGLE(1, 1);
    auto maxResolution = SPA_RECTANGLE(8192, 8192);

    auto framerate = SPA_FRACTION(25, 1);

    uint8_t buffer[1024];
    spa_pod_builder podBuilder = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

    const spa_pod *params[1];
    params[0] = static_cast<struct spa_pod *>(
                spa_pod_builder_add_object(
                    &podBuilder,
                    SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
                    SPA_FORMAT_mediaType, SPA_POD_Id(SPA_MEDIA_TYPE_video),
                    SPA_FORMAT_mediaSubtype, SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
                    SPA_FORMAT_VIDEO_format, SPA_POD_Id(drmFormatToSpaFormat(frameSet->format)),
                    SPA_FORMAT_VIDEO_modifier, SPA_POD_Id(frameSet->modifier),
                    SPA_FORMAT_VIDEO_size, SPA_POD_CHOICE_RANGE_Rectangle(&resolution, &minResolution, &maxResolution),
                    SPA_FORMAT_VIDEO_framerate, SPA_POD_Fraction(&framerate))
                );

    pw_stream_add_listener(pwStream, &pwStreamListener, &pwStreamEvents, this);

    auto flags = static_cast<pw_stream_flags>(PW_STREAM_FLAG_DRIVER | PW_STREAM_FLAG_ALLOC_BUFFERS);
    auto result = pw_stream_connect(pwStream, PW_DIRECTION_OUTPUT, SPA_ID_INVALID, flags, params, G_N_ELEMENTS(params));
    if (result != 0) {
        qCWarning(lcScreenCast, "Could not connect to PipeWire stream: %s",
                  spa_strerror(result));
        return false;
    }

    ///

    if (pw_thread_loop_start(pwMainLoop) != 0) {
        qCWarning(lcScreenCast, "Failed to start PipeWire loop");
        return false;
    }

    return true;
}

void ScreenCastStream::removeStream()
{
    if (pwMainLoop)
        pw_thread_loop_stop(pwMainLoop);

    if (pwStream) {
        pw_stream_destroy(pwStream);
        pwStream = nullptr;
    }

    if (pwCore) {
        pw_core_disconnect(pwCore);
        pwCore = nullptr;
    }

    if (pwContext) {
        pw_context_destroy(pwContext);
        pwContext = nullptr;
    }

    if (pwMainLoop) {
        pw_thread_loop_destroy(pwMainLoop);
        pwMainLoop = nullptr;
    }
}

void ScreenCastStream::freeFrame()
{
    frameSet->reset();
    m_frame->deleteLater();
    m_frame = nullptr;
}

void ScreenCastStream::handleFrame(const QSize &size, const QPoint &offset,
                                   WlrExportDmabufFrameV1::BufferFlags bufferFlags,
                                   WlrExportDmabufFrameV1::FrameFlags flags,
                                   quint32 format, quint64 modifier,
                                   quint32 numObjects)
{
    Q_UNUSED(bufferFlags)
    Q_UNUSED(flags)
    Q_UNUSED(modifier)

    frameSet->size = size;
    frameSet->offset = offset;
    frameSet->format = format;
    frameSet->modifier = modifier;
    frameSet->objects.reserve(numObjects);
}

void ScreenCastStream::handleObject(quint32 index, quint32 fd, quint32 size,
                                    quint32 offset, quint32 stride,
                                    quint32 planeIndex)
{
    Q_UNUSED(index)
    Q_UNUSED(planeIndex)

    auto *frameObject = new FrameObject;
    frameObject->fd = fd;
    frameObject->size = size;
    frameObject->offset = offset;
    frameObject->stride = stride;
    frameSet->objects.append(frameObject);
}

void ScreenCastStream::handleReady(quint64 tv_sec, quint32 tv_nsec)
{
    frameSet->tv_sec = tv_sec;
    frameSet->tv_nsec = tv_nsec;
    frameSet->ready = true;

    Q_EMIT frameSetReady();
}

void ScreenCastStream::handleCancel(WlrExportDmabufFrameV1::CancelReason reason)
{
    frameSet->reset();

    if (reason == WlrExportDmabufFrameV1::Permanent)
        qCWarning(lcScreenCast, "Permanent failure");
}
