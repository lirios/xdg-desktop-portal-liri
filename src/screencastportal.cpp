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

#include "screencastportal.h"
#include "logging_p.h"

ScreenCastPortal::ScreenCastPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint ScreenCastPortal::CreateSession(const QDBusObjectPath &handle,
                                     const QDBusObjectPath &session_handle,
                                     const QString &app_id,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    qCDebug(lcScreenCast) << "CreateSession called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    options: " << options;
}

uint ScreenCastPortal::SelectSources(const QDBusObjectPath &handle,
                                     const QDBusObjectPath &session_handle,
                                     const QString &app_id,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    qCDebug(lcScreenCast) << "SelectSource called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    options: " << options;
}

uint ScreenCastPortal::Start(const QDBusObjectPath &handle,
                             const QDBusObjectPath &session_handle,
                             const QString &app_id,
                             const QString &parent_window,
                             const QVariantMap &options,
                             QVariantMap &results)
{
    qCDebug(lcScreenCast) << "Start called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    parent_window: " << parent_window;
    qCDebug(lcScreenCast) << "    options: " << options;
}
