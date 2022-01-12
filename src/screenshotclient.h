// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENSHOTCLIENT_H
#define SCREENSHOTCLIENT_H

#include <QObject>
#include <QImage>

#include <LiriAuroraClient/WlrScreencopyManagerV1>

class ScreenshotClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
public:
    enum What {
        AllScreens = 0,
        Area
    };
    Q_ENUM(What)

    explicit ScreenshotClient(QObject *parent = nullptr);

    bool isEnabled() const;

    QImage image() const;

    Q_INVOKABLE QString generateFileName() const;

    Q_INVOKABLE void takeScreenshot(ScreenshotClient::What what, bool overlayCursor);
    Q_INVOKABLE bool saveScreenshot(const QUrl &url) const;
    Q_INVOKABLE void copyToClipboard() const;

Q_SIGNALS:
    void enabledChanged();
    void screenshotDone();

private:
    bool m_enabled = false;
    bool m_interactive = false;
    bool m_inProgress = false;
    int m_screensToGo = 0;
    QImage m_finalImage;
    Aurora::Client::WlrScreencopyManagerV1 *m_screencopy = nullptr;

    void done();

private Q_SLOTS:
    void handleFrameCopied(const QImage &image);
};

#endif // SCREENSHOTCLIENT_H
