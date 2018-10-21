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
