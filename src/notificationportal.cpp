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

#include "notificationportal.h"
#include "logging_p.h"

NotificationPortal::NotificationPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

void NotificationPortal::AddNotification(const QString &app_id,
                                         const QString &id,
                                         const QVariantMap &notification)
{
    qCDebug(lcNotification) << "AddNotification called with parameters:";
    qCDebug(lcNotification) << "    app_id: " << app_id;
    qCDebug(lcNotification) << "    id: " << id;
    qCDebug(lcNotification) << "    notification: " << notification;
}

void NotificationPortal::RemoveNotification(const QString &app_id,
                                            const QString &id)
{
    qCDebug(lcNotification) << "RemoveNotification called with parameters:";
    qCDebug(lcNotification) << "    app_id: " << app_id;
    qCDebug(lcNotification) << "    id: " << id;
}
