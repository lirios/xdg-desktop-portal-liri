// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCoreApplication>
#include <QQmlContext>
#include <QQmlEngine>

#include "accessportal.h"
#include "accountportal.h"
#include "appchooserportal.h"
#include "backgroundportal.h"
#include "desktopportal.h"
#include "emailportal.h"
#include "filechooserportal.h"
#include "inhibitportal.h"
#include "lockdownportal.h"
#include "notificationportal.h"
#include "printportal.h"
#include "screenshotclient.h"
#include "screenshotimageprovider.h"
#include "screencastportal.h"
#include "screenshotportal.h"
#include "settingsportal.h"
#include "wallpaperportal.h"

Q_GLOBAL_STATIC(DesktopPortal, sDesktopPortal)

DesktopPortal::DesktopPortal(QObject *parent)
    : QObject(parent)
    , m_engine(new QQmlEngine(this))
    , m_access(new AccessPortal(this))
    , m_account(new AccountPortal(this))
    , m_appChooser(new AppChooserPortal(this))
    , m_background(new BackgroundPortal(this))
    , m_email(new EmailPortal(this))
    , m_fileChooser(new FileChooserPortal(this))
    , m_inhibit(new InhibitPortal(this))
    , m_lockdown(new LockdownPortal(this))
    , m_notification(new NotificationPortal(this))
    , m_print(new PrintPortal(this))
    , m_screenCast(new ScreenCastPortal(this))
    , m_screenshot(new ScreenshotPortal(this))
    , m_settings(new SettingsPortal(this))
    , m_wallpaper(new WallpaperPortal(this))
    , m_screenshotClient(new ScreenshotClient(this))
    , m_screenshotImageProvider(new ScreenshotImageProvider())
{
    m_engine->addImageProvider(QStringLiteral("screenshooter"), m_screenshotImageProvider);
    m_engine->rootContext()->setContextProperty(QStringLiteral("Screenshooter"), m_screenshotClient);

    connect(m_engine, &QQmlEngine::quit,
            QCoreApplication::instance(), &QCoreApplication::quit);
    connect(m_engine, &QQmlEngine::exit,
            QCoreApplication::instance(), &QCoreApplication::exit);

    connect(m_screenshotClient, &ScreenshotClient::screenshotDone, this, [this] {
        m_screenshotImageProvider->image = m_screenshotClient->image();
    });
}

QQmlEngine *DesktopPortal::engine() const
{
    return m_engine;
}

ScreenshotClient *DesktopPortal::screenshotClient() const
{
    return m_screenshotClient;
}

DesktopPortal *DesktopPortal::instance()
{
    return sDesktopPortal();
}
