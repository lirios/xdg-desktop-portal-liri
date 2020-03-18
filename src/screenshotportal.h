// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENSHOTPORTAL_H
#define SCREENSHOTPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class ScreenshotPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Screenshot")
public:
    struct ColorRGB {
        double red;
        double green;
        double blue;
    };

    explicit ScreenshotPortal(QObject *parent);

public Q_SLOTS:
    quint32 Screenshot(const QDBusObjectPath &handle,
                       const QString &app_id,
                       const QString &parent_window,
                       const QVariantMap &options,
                       QVariantMap &results);
    quint32 PickColor(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QVariantMap &options,
                      QVariantMap &results);
};

#endif // SCREENSHOTPORTAL_H
