// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QClipboard>
#include <QDateTime>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QScreen>
#include <QStandardPaths>
#include <QUrl>

#include "logging_p.h"
#include "screenshotclient.h"

ScreenshotClient::ScreenshotClient(QObject *parent)
    : QObject(parent)
    , m_screencopy(new Aurora::Client::WlrScreencopyManagerV1())
{
    if (m_screencopy->isActive()) {
        m_enabled = true;
        Q_EMIT enabledChanged();
    }

    connect(m_screencopy, &Aurora::Client::WlrScreencopyManagerV1::activeChanged, this, [this] {
        bool oldValue = m_enabled;

        if (!oldValue && m_screencopy->isActive())
            m_enabled = true;
        else if (oldValue && !m_screencopy->isActive())
            m_enabled = false;

        if (oldValue != m_enabled)
            Q_EMIT enabledChanged();
    });
}

bool ScreenshotClient::isEnabled() const
{
    return m_enabled;
}

QImage ScreenshotClient::image() const
{
    return m_finalImage;
}

void ScreenshotClient::done()
{
    Q_EMIT screenshotDone();

    m_inProgress = false;
}

void ScreenshotClient::handleFrameCopied(const QImage &image)
{
    auto *frame = qobject_cast<Aurora::Client::WlrScreencopyFrameV1 *>(sender());
    if (!frame)
        return;

    QPainter painter(&m_finalImage);
    painter.drawImage(frame->screen()->geometry().topLeft(), image);

    m_screensToGo--;

    if (m_screensToGo == 0)
        done();
}

QString ScreenshotClient::generateFileName() const
{
    return QStringLiteral("%1/%2.png")
            .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                 tr("Screenshot from %1").arg(QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"))));
}

void ScreenshotClient::takeScreenshot(What what, bool overlayCorsor)
{
    if (!m_enabled) {
        qCWarning(lcScreenshot, "We are not ready to take screenshots");
        return;
    }

    if (m_inProgress) {
        qCWarning(lcScreenshot, "Cannot take another screenshot while a previous capture is in progress");
        return;
    }

    m_inProgress = true;

    switch (what) {
    case AllScreens: {
        const auto screens = qGuiApp->screens();
        m_screensToGo = screens.size();

        QRect screensGeometry;
        for (auto *screen : screens)
            screensGeometry |= screen->geometry();

        m_finalImage = QImage(screensGeometry.size(), QImage::Format_RGB32);
        m_finalImage.fill(Qt::black);

        for (auto *screen : screens) {
            auto *frame = m_screencopy->captureScreen(screen, overlayCorsor);
            connect(frame, &Aurora::Client::WlrScreencopyFrameV1::copied, this, &ScreenshotClient::handleFrameCopied);
        }
        break;
    }
        break;
    case Area:
        // TODO: We need a protocol to select an area and then we call m_screencopy->captureScreenRegion()
        break;
    }
}

bool ScreenshotClient::saveScreenshot(const QUrl &url) const
{
    if (m_finalImage.save(url.toLocalFile())) {
        qCInfo(lcScreenshot, "Screenshot saved to \"%s\" successfully", qUtf8Printable(url.toLocalFile()));
        return true;
    }

    qCWarning(lcScreenshot, "Failed to save screenshot to \"%s\"", qUtf8Printable(url.toLocalFile()));
    return false;
}

void ScreenshotClient::copyToClipboard() const
{
    qGuiApp->clipboard()->setImage(m_finalImage);
}
