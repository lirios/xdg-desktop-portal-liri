// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H

#include <QObject>

#include <LiriWaylandClient/LiriColorPickerManager>
#include <LiriWaylandClient/WlrExportDmabufManagerV1>
#include <LiriWaylandClient/WlrForeignToplevelManagerV1>

QT_FORWARD_DECLARE_CLASS(QScreen)

class ScreenCastStream;

class WaylandIntegration : public QObject
{
    Q_OBJECT
public:
    typedef struct {
        quint32 nodeId = 0;
        QVariantMap map;
    } Stream;
    typedef QVector<Stream> Streams;

    explicit WaylandIntegration(QObject *parent = nullptr);
    ~WaylandIntegration();

    LiriColorPickerManager *colorPicker() const;

    QVector<WlrForeignToplevelHandleV1 *> toplevels() const;

#ifdef SCREENCAST_ENABLED
    QVariant streams();

    bool startStreaming(QScreen *screen);
    void stopStreaming(QScreen *screen);
#endif

    static WaylandIntegration *instance();

Q_SIGNALS:
    void toplevelsChanged();

private:
    LiriColorPickerManager *m_colorPicker = nullptr;
    WlrForeignToplevelManagerV1 *m_toplevelManager = nullptr;
    QVector<WlrForeignToplevelHandleV1 *> m_toplevels;

    WlrExportDmabufManagerV1 *m_exportDmabuf = nullptr;
#ifdef SCREENCAST_ENABLED
    QMap<QScreen *, WlrExportDmabufFrameV1 *> m_frames;
    QMap<QScreen *, ScreenCastStream *> m_streams;

    bool startStreamingImmediately(QScreen *screen);
#endif

private Q_SLOTS:
    void handleToplevel(WlrForeignToplevelHandleV1 *toplevel);
};

#endif // WAYLANDINTEGRATION_H
