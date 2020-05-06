// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2016 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTIFICATIONPORTAL_H
#define NOTIFICATIONPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

#include <LiriNotifications/Notification>

class NotificationPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Notification")
public:
    explicit NotificationPortal(QObject *parent);

public Q_SLOTS:
    void AddNotification(const QString &app_id,
                         const QString &id,
                         const QVariantMap &notification);
    void RemoveNotification(const QString &app_id,
                            const QString &id);

private:
    QMap<QString, Liri::Notification *> m_notifications;

    QString findKey(Liri::Notification *notify, QString &appId, QString &id) const;

private Q_SLOTS:
    void notificationActionInvoked(const QString &action);
    void notificationClosed(Liri::Notification::CloseReason reason);
};

#endif // NOTIFICATIONPORTAL_H
