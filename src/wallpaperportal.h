// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WALLPAPERPORTAL_H
#define WALLPAPERPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class WallpaperPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Wallpaper")
public:
    explicit WallpaperPortal(QObject *parent);

public Q_SLOTS:
    quint32 SetWallpaperURI(const QDBusObjectPath &handle,
                            const QString &app_id,
                            const QString &parent_window,
                            const QString &uri,
                            const QVariantMap &options);

private:
    void setWallpaper(const QString &setOn, const QString &uri);
    void setOnBackground(const QString &uri);
    void setOnLockscreen(const QString &uri);
};

#endif // WALLPAPERPORTAL_H
