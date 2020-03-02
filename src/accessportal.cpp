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

#include "accessportal.h"
#include "logging_p.h"

AccessPortal::AccessPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint AccessPortal::AccessDialog(const QDBusObjectPath &handle, const QString &app_id,
                                const QString &parent_window, const QString &title,
                                const QString &subtitle, const QString &body,
                                const QVariantMap &options, QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcAccess) << "AccessDialog called with parameters:";
    qCDebug(lcAccess) << "    handle: " << handle.path();
    qCDebug(lcAccess) << "    app_id: " << app_id;
    qCDebug(lcAccess) << "    parent_window: " << parent_window;
    qCDebug(lcAccess) << "    title: " << title;
    qCDebug(lcAccess) << "    subtitle: " << subtitle;
    qCDebug(lcAccess) << "    body: " << body;
    qCDebug(lcAccess) << "    options: " << options;

    return 1;
}
