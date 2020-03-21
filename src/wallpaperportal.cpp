// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQuickItem>

#include <Qt5GSettings/Qt5GSettings>

#include "quickdialog.h"
#include "request.h"
#include "logging_p.h"
#include "wallpaperportal.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Wallpaper.xml

WallpaperPortal::WallpaperPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

quint32 WallpaperPortal::SetWallpaperURI(const QDBusObjectPath &handle,
                                         const QString &app_id,
                                         const QString &parent_window,
                                         const QString &uri,
                                         const QVariantMap &options)
{
    qCDebug(lcWallpaper) << "SetWallpaperURI called with parameters:";
    qCDebug(lcWallpaper) << "    handle: " << handle.path();
    qCDebug(lcWallpaper) << "    app_id: " << app_id;
    qCDebug(lcWallpaper) << "    parent_window: " << parent_window;
    qCDebug(lcWallpaper) << "    uri: " << uri;
    qCDebug(lcWallpaper) << "    options: " << options;

    const bool showPreview = options.value(QStringLiteral("show-preview"), true).toBool();
    const QString setOn = options.value(QStringLiteral("set-on"), QStringLiteral("both")).toString();

    if (showPreview) {
        auto *dialog = new QuickDialog(QUrl(QLatin1String("qrc:/qml/WallpaperDialog.qml")));
        dialog->rootObject()->setProperty("setOn", setOn);
        dialog->rootObject()->setProperty("uri", uri);
        if (dialog->exec()) {
            setWallpaper(setOn, uri);
            dialog->deleteLater();
            return 0;
        }

        dialog->deleteLater();
        return 1;
    } else {
        setWallpaper(setOn, uri);
        return 0;
    }

    return 1;
}

void WallpaperPortal::setWallpaper(const QString &setOn, const QString &uri)
{
    if (setOn == QLatin1String("background")) {
        setOnBackground(uri);
    } else if (setOn == QLatin1String("lockscreen")) {
        setOnLockscreen(uri);
    } else {
        setOnBackground(uri);
        setOnLockscreen(uri);
    }
}

void WallpaperPortal::setOnBackground(const QString &uri)
{
    auto *settings = new QtGSettings::QGSettings(QStringLiteral("io.liri.desktop.background"),
                                                 QStringLiteral("/io/liri/desktop/background/"));
    settings->setValue(QStringLiteral("picture-url"), QUrl(uri));
    settings->deleteLater();
}

void WallpaperPortal::setOnLockscreen(const QString &uri)
{
    auto *settings = new QtGSettings::QGSettings(QStringLiteral("io.liri.desktop.lockscreen"),
                                                 QStringLiteral("/io/liri/desktop/lockscreen/"));
    settings->setValue(QStringLiteral("picture-url"), QUrl(uri));
    settings->deleteLater();
}
