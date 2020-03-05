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

#include "inhibitportal.h"
#include "logging_p.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Inhibit.xml

InhibitPortal::InhibitPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

void InhibitPortal::Inhibit(const QDBusObjectPath &handle,
                            const QString &app_id,
                            const QString &window,
                            uint flags,
                            const QVariantMap &options)
{
    qCDebug(lcInhibit) << "Inhibit called with parameters:";
    qCDebug(lcInhibit) << "    handle: " << handle.path();
    qCDebug(lcInhibit) << "    app_id: " << app_id;
    qCDebug(lcInhibit) << "    window: " << window;
    qCDebug(lcInhibit) << "    flags: " << flags;
    qCDebug(lcInhibit) << "    options: " << options;
}
