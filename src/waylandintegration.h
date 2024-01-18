// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H

#include <QObject>
#include <QMap>

#include <LiriAuroraClient/LiriColorPickerManagerV1>

QT_FORWARD_DECLARE_CLASS(QScreen)

class ScreenCastStream;

class WaylandIntegration : public QObject
{
    Q_OBJECT
public:
    struct Stream {
        quint32 nodeId = 0;
        QVariantMap map;
    };
    typedef QVector<Stream> Streams;

    explicit WaylandIntegration(QObject *parent = nullptr);
    ~WaylandIntegration();

    Aurora::Client::LiriColorPickerManagerV1 *colorPicker() const;

    QVector<Aurora::Client::WlrForeignToplevelHandleV1 *> toplevels() const;

#ifdef SCREENCAST_ENABLED
    QVariant streams();

    bool startStreaming(QScreen *screen);
    void stopStreaming(QScreen *screen);
#endif

    static WaylandIntegration *instance();

Q_SIGNALS:
    void toplevelsChanged();

private:
    Aurora::Client::LiriColorPickerManagerV1 *m_colorPicker = nullptr;
    Aurora::Client::WlrForeignToplevelManagerV1 *m_toplevelManager = nullptr;
    QVector<Aurora::Client::WlrForeignToplevelHandleV1 *> m_toplevels;

    Aurora::Client::WlrExportDmabufManagerV1 *m_exportDmabuf = nullptr;
#ifdef SCREENCAST_ENABLED
    QMap<QScreen *, Aurora::Client::WlrExportDmabufFrameV1 *> m_frames;
    QMap<QScreen *, ScreenCastStream *> m_streams;

    bool startStreamingImmediately(QScreen *screen);
#endif

private Q_SLOTS:
    void handleToplevel(Aurora::Client::WlrForeignToplevelHandleV1 *toplevel);
};

#endif // WAYLANDINTEGRATION_H
