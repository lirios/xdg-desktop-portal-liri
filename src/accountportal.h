// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ACCOUNTPORTAL_H
#define ACCOUNTPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class AccountPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Account")
public:
    explicit AccountPortal(QObject *parent);

public Q_SLOTS:
    quint32 GetUserInformation(const QDBusObjectPath &handle,
                               const QString &app_id,
                               const QString &parent_window,
                               const QVariantMap &options,
                               QVariantMap &results);
};

#endif // ACCOUNTPORTAL_H
