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

#include "filechooserportal.h"
#include "logging_p.h"

FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint FileChooserPortal::OpenFile(const QDBusObjectPath &handle,
                                 const QString &app_id,
                                 const QString &parent_window,
                                 const QString &title,
                                 const QVariantMap &options,
                                 QVariantMap &results)
{
    qCDebug(lcFileChooser) << "OpenFile called with parameters:";
    qCDebug(lcFileChooser) << "    handle: " << handle.path();
    qCDebug(lcFileChooser) << "    parent_window: " << parent_window;
    qCDebug(lcFileChooser) << "    title: " << title;
    qCDebug(lcFileChooser) << "    options: " << options;
}

uint FileChooserPortal::SaveFile(const QDBusObjectPath &handle,
                                 const QString &app_id,
                                 const QString &parent_window,
                                 const QString &title,
                                 const QVariantMap &options,
                                 QVariantMap &results)
{
    qCDebug(lcFileChooser) << "SaveFile called with parameters:";
    qCDebug(lcFileChooser) << "    handle: " << handle.path();
    qCDebug(lcFileChooser) << "    parent_window: " << parent_window;
    qCDebug(lcFileChooser) << "    title: " << title;
    qCDebug(lcFileChooser) << "    options: " << options;
}
