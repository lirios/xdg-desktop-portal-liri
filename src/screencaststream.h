// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENCASTSTREAM_H
#define SCREENCASTSTREAM_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include <LiriAuroraClient/WlrExportDmabufManagerV1>

#include <spa/param/video/format-utils.h>
#include <spa/param/props.h>

#include <pipewire/pipewire.h>

class FrameObject
{
public:
    FrameObject() = default;

    int fd = -1;
    quint32 size = 0;
    quint32 offset = 0;
    quint32 stride = 0;
};

class FrameSet
{
public:
    FrameSet() = default;
    ~FrameSet();

    void reset();

    QSize size;
    QPoint offset;
    QVector<FrameObject *> objects;
    quint32 format = 0;
    quint64 modifier = 0;
    quint64 tv_sec = 0;
    quint32 tv_nsec = 0;
    bool ready = false;
};

class ScreenCastStream : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
public:
    explicit ScreenCastStream(Aurora::Client::WlrExportDmabufFrameV1 *frame, QObject *parent = nullptr);
    ~ScreenCastStream();

    quint32 nodeId() const;
    void setNodeId(quint32 nodeId);

    bool createStream();
    void removeStream();

    void freeFrame();

Q_SIGNALS:
    void nodeIdChanged();
    void frameSetReady();
    void streamReady(quint32 nodeId);
    void streamStarted();
    void streamStopped();

public:
    FrameSet *frameSet = nullptr;

    pw_context *pwContext = nullptr;
    pw_core *pwCore = nullptr;
    pw_loop *pwLoop = nullptr;
    pw_thread_loop *pwMainLoop = nullptr;
    pw_stream *pwStream = nullptr;

    spa_video_info_raw pwInfo;
    spa_hook pwCoreListener;
    spa_hook pwStreamListener;

    bool isStreaming = false;
    quint32 m_nodeId = SPA_ID_INVALID;

private:
    Aurora::Client::WlrExportDmabufFrameV1 *m_frame = nullptr;

private Q_SLOTS:
    void handleFrame(const QSize &size, const QPoint &offset,
                     Aurora::Client::WlrExportDmabufFrameV1::BufferFlags bufferFlags,
                     Aurora::Client::WlrExportDmabufFrameV1::FrameFlags flags,
                     quint32 format, quint64 modifier,
                     quint32 numObjects);
    void handleObject(quint32 index, quint32 fd, quint32 size,
                      quint32 offset, quint32 stride,
                      quint32 planeIndex);
    void handleReady(quint64 tv_sec, quint32 tv_nsec);
    void handleCancel(Aurora::Client::WlrExportDmabufFrameV1::CancelReason reason);
};

#endif // SCREENCASTSTREAM_H
