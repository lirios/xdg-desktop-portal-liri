/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

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
