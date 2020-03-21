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

#include <QDBusConnection>
#include <QApplication>
#include <QQmlComponent>

#include "desktopportal.h"
#include "logging_p.h"

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Setup the environment
    qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    qputenv("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT", QByteArray("1"));

    // Application
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("XDG Desktop Portal"));
    app.setApplicationVersion(QStringLiteral(VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));
#ifndef QT_NO_SESSIONMANAGER
    app.setFallbackSessionManagementEnabled(false);
#endif
    app.setQuitOnLastWindowClosed(false);

    // Register service
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (sessionBus.registerService(QStringLiteral("org.freedesktop.impl.portal.desktop.liri"))) {
        auto *desktopPortal = DesktopPortal::instance();
        if (sessionBus.registerObject(QStringLiteral("/org/freedesktop/portal/desktop"), desktopPortal, QDBusConnection::ExportAdaptors)) {
            qCDebug(lcPortal) << "XDG desktop portal for Liri registered";
        } else {
            qCWarning(lcPortal) << "Failed to register XDG desktop portal for Liri";
            return 1;
        }
    } else {
        qCWarning(lcPortal) << "Failed to register org.freedesktop.impl.portal.desktop.liri service";
        return 1;
    }

    return app.exec();
}
