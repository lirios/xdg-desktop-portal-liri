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

#ifndef DESKTOPPORTAL_H
#define DESKTOPPORTAL_H

#include <QObject>

class QQmlEngine;

class AccessPortal;
class AccountPortal;
class AppChooserPortal;
class BackgroundPortal;
class EmailPortal;
class FileChooserPortal;
class InhibitPortal;
class LockdownPortal;
class NotificationPortal;
class PrintPortal;
class ScreenCastPortal;
class ScreenshotClient;
class ScreenshotImageProvider;
class ScreenshotPortal;
class SettingsPortal;
class WallpaperPortal;

class DesktopPortal : public QObject
{
    Q_OBJECT
public:
    explicit DesktopPortal(QObject *parent = nullptr);

    QQmlEngine *engine() const;
    ScreenshotClient *screenshotClient() const;

    static DesktopPortal *instance();

private:
    QQmlEngine *m_engine = nullptr;

    AccessPortal *m_access = nullptr;
    AccountPortal *m_account = nullptr;
    AppChooserPortal *m_appChooser = nullptr;
    BackgroundPortal *m_background = nullptr;
    EmailPortal *m_email = nullptr;
    FileChooserPortal *m_fileChooser = nullptr;
    InhibitPortal *m_inhibit = nullptr;
    LockdownPortal *m_lockdown = nullptr;
    NotificationPortal *m_notification = nullptr;
    PrintPortal *m_print = nullptr;
    ScreenCastPortal *m_screenCast = nullptr;
    ScreenshotPortal *m_screenshot = nullptr;
    SettingsPortal *m_settings = nullptr;
    WallpaperPortal *m_wallpaper = nullptr;

    ScreenshotClient *m_screenshotClient = nullptr;
    ScreenshotImageProvider *m_screenshotImageProvider = nullptr;
};

#endif // DESKTOPPORTAL_H
