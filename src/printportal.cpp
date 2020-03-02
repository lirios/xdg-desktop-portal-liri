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

#include "printportal.h"
#include "logging_p.h"

PrintPortal::PrintPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint PrintPortal::Print(const QDBusObjectPath &handle,
                        const QString &app_id,
                        const QString &parent_window,
                        const QString &title,
                        const QDBusUnixFileDescriptor &fd,
                        const QVariantMap &options,
                        QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcPrint) << "Print called with parameters:";
    qCDebug(lcPrint) << "    handle: " << handle.path();
    qCDebug(lcPrint) << "    app_id: " << app_id;
    qCDebug(lcPrint) << "    parent_window: " << parent_window;
    qCDebug(lcPrint) << "    title: " << title;
    qCDebug(lcPrint) << "    fd: " << fd.fileDescriptor();
    qCDebug(lcPrint) << "    options: " << options;

    return 1;
}

uint PrintPortal::PreparePrint(const QDBusObjectPath &handle,
                               const QString &app_id,
                               const QString &parent_window,
                               const QString &title,
                               const QVariantMap &settings,
                               const QVariantMap &page_setup,
                               const QVariantMap &options,
                               QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcPrint) << "PreparePrint called with parameters:";
    qCDebug(lcPrint) << "    handle: " << handle.path();
    qCDebug(lcPrint) << "    app_id: " << app_id;
    qCDebug(lcPrint) << "    parent_window: " << parent_window;
    qCDebug(lcPrint) << "    title: " << title;
    qCDebug(lcPrint) << "    settings: " << settings;
    qCDebug(lcPrint) << "    page_setup: " << page_setup;
    qCDebug(lcPrint) << "    options: " << options;

    return 1;
}
