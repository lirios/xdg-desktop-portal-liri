// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EMAILPORTAL_H
#define EMAILPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class EmailPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Email")
public:
    explicit EmailPortal(QObject *parent);

public Q_SLOTS:
    quint32 ComposeEmail(const QDBusObjectPath &handle,
                         const QString &app_id,
                         const QString &window,
                         const QVariantMap &options,
                         QVariantMap &results);
};

#endif // EMAILPORTAL_H
