/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QEventLoop>
#include <QScreen>
#include <QTimer>

#include "logging_p.h"
#include "waylandintegration.h"
#ifdef SCREENCAST_ENABLED
#  include "screencaststream.h"
#endif

Q_GLOBAL_STATIC(WaylandIntegration, s_waylandIntegration)

const QDBusArgument &operator>>(const QDBusArgument &arg, WaylandIntegration::Stream &stream)
{
    arg.beginStructure();
    arg >> stream.nodeId;

    arg.beginMap();
    while (!arg.atEnd()) {
        QString key;
        QVariant map;
        arg.beginMapEntry();
        arg >> key >> map;
        arg.endMapEntry();
        stream.map.insert(key, map);
    }
    arg.endMap();
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator<<(QDBusArgument &arg, const WaylandIntegration::Stream &stream)
{
    arg.beginStructure();
    arg << stream.nodeId;
    arg << stream.map;
    arg.endStructure();

    return arg;
}

Q_DECLARE_METATYPE(WaylandIntegration::Stream)
Q_DECLARE_METATYPE(WaylandIntegration::Streams)

WaylandIntegration::WaylandIntegration(QObject *parent)
    : QObject(parent)
    , m_exportDmabuf(new WlrExportDmabufManagerV1)
{
    qDBusRegisterMetaType<WaylandIntegration::Stream>();
    qDBusRegisterMetaType<WaylandIntegration::Streams>();
}

WaylandIntegration::~WaylandIntegration()
{
    m_exportDmabuf->deleteLater();
#ifdef SCREENCAST_ENABLED
    qDeleteAll(m_streams);
#endif
}

WaylandIntegration *WaylandIntegration::instance()
{
    return s_waylandIntegration();
}

#ifdef SCREENCAST_ENABLED
QVariant WaylandIntegration::streams()
{
    Streams streams;

    for (auto *screen : m_streams.keys()) {
        QVariantMap map;
        map.insert(QStringLiteral("position"), screen->geometry().topLeft());
        map.insert(QStringLiteral("size"), screen->size());

        Stream stream;
        stream.nodeId = m_streams[screen]->nodeId();
        stream.map = map;
        streams.append(stream);
    }

    return QVariant::fromValue<Streams>(streams);
}

bool WaylandIntegration::startStreaming(QScreen *screen)
{
    if (m_exportDmabuf->isActive()) {
        return startStreamingImmediately(screen);
    } else {
        qCWarning(lcWaylandIntegration) << "Export dmabuf extension is not ready, waiting...";

        bool result = false;

        connect(m_exportDmabuf, &WlrExportDmabufManagerV1::activeChanged,
                this, [this, screen, &result] {
            qCInfo(lcWaylandIntegration) << "Export dmabuf extension is ready";
            result = startStreamingImmediately(screen);
        });

        // Wait until it's active
        QEventLoop loop;
        connect(m_exportDmabuf, &WlrExportDmabufManagerV1::activeChanged,
                &loop, &QEventLoop::quit);
        QTimer::singleShot(3000, &loop, &QEventLoop::quit);
        loop.exec();

        return result;
    }
}

void WaylandIntegration::stopStreaming(QScreen *screen)
{
    if (m_streams.contains(screen)) {
        m_streams[screen]->removeStream();
        m_streams[screen]->deleteLater();
        m_streams.remove(screen);
    }

    if (m_frames.contains(screen)) {
        m_frames[screen]->deleteLater();
        m_frames.remove(screen);
    }
}

bool WaylandIntegration::startStreamingImmediately(QScreen *screen)
{
    if (m_frames.contains(screen)) {
        m_frames[screen]->deleteLater();
        m_frames.remove(screen);
    }

    // TODO: overlay cursor?
    auto *frame = m_exportDmabuf->captureOutput(true, screen);
    if (frame) {
        m_frames[screen] = frame;
        m_streams[screen] = new ScreenCastStream(frame);

        // Wait until it's ready
        if (!m_streams[screen]->frameSet->ready) {
            qCDebug(lcWaylandIntegration) << "Frame set is not ready, waiting...";

            QEventLoop loop;
            connect(m_streams[screen], &ScreenCastStream::frameSetReady,
                    &loop, &QEventLoop::quit);
            QTimer::singleShot(5000, &loop, &QEventLoop::quit);
            loop.exec();

            if (m_streams[screen]->frameSet->ready) {
                qCInfo(lcWaylandIntegration) << "Frame set is ready";
            } else {
                qCWarning(lcWaylandIntegration) << "Frame set is not ready!";
                return false;
            }
        }

        // Start streaming
        if (!m_streams[screen]->createStream())
            return false;

        // Wait until streaming is ready
        if (!m_streams[screen]->isStreaming) {
            qCDebug(lcWaylandIntegration) << "Stream is preparing...";

            QEventLoop loop;
            connect(m_streams[screen], &ScreenCastStream::streamReady,
                    &loop, &QEventLoop::quit);
            QTimer::singleShot(25000, &loop, &QEventLoop::quit);
            loop.exec();

            if (m_streams[screen]->isStreaming) {
                qCInfo(lcWaylandIntegration) << "Stream is ready";
            } else {
                qCWarning(lcWaylandIntegration) << "Stream is not ready!";
                return false;
            }
        }

        return true;
    } else {
        qCWarning(lcWaylandIntegration) << "Unable to capture screen";
    }

    return false;
}
#endif
