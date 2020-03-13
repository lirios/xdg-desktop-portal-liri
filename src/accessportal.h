// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ACCESSPORTAL_H
#define ACCESSPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class AccessPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Access")
public:
    explicit AccessPortal(QObject *parent);

public Q_SLOTS:
    quint32 AccessDialog(const QDBusObjectPath &handle,
                         const QString &app_id,
                         const QString &parent_window,
                         const QString &title,
                         const QString &subtitle,
                         const QString &body,
                         const QVariantMap &options,
                         QVariantMap &results);
};

#endif // ACCESSPORTAL_H
