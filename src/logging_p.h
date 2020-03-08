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

#ifndef LOGGING_P_H
#define LOGGING_P_H

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcPortal)
Q_DECLARE_LOGGING_CATEGORY(lcAccess)
Q_DECLARE_LOGGING_CATEGORY(lcAccount)
Q_DECLARE_LOGGING_CATEGORY(lcAppChooser)
Q_DECLARE_LOGGING_CATEGORY(lcEmail)
Q_DECLARE_LOGGING_CATEGORY(lcFileChooser)
Q_DECLARE_LOGGING_CATEGORY(lcInhibit)
Q_DECLARE_LOGGING_CATEGORY(lcNotification)
Q_DECLARE_LOGGING_CATEGORY(lcPrint)
Q_DECLARE_LOGGING_CATEGORY(lcRequest)
Q_DECLARE_LOGGING_CATEGORY(lcScreenCast)
Q_DECLARE_LOGGING_CATEGORY(lcScreenshot)
Q_DECLARE_LOGGING_CATEGORY(lcSession)
Q_DECLARE_LOGGING_CATEGORY(lcWallpaper)
Q_DECLARE_LOGGING_CATEGORY(lcWaylandIntegration)

#endif // LOGGING_P_H
