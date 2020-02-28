/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H

#include <QObject>

#include <LiriWaylandClient/WlrExportDmabufV1>

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

    QVariant streams();

    bool startStreaming(QScreen *screen);
    void stopStreaming(QScreen *screen);

    static WaylandIntegration *instance();

private:
    WlrExportDmabufManagerV1 *m_exportDmabuf = nullptr;
    QMap<QScreen *, WlrExportDmabufFrameV1 *> m_frames;
    QMap<QScreen *, ScreenCastStream *> m_streams;

    bool startStreamingImmediately(QScreen *screen);
};

#endif // WAYLANDINTEGRATION_H
