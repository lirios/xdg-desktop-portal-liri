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
#include <QGuiApplication>
#include <QQmlComponent>

#include "desktopportal.h"
#include "logging_p.h"
#include "quickdialog.h"

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QLatin1String("XDG Desktop Portal"));
    app.setApplicationVersion(QLatin1String(VERSION));
    app.setOrganizationName(QLatin1String("Liri"));
    app.setOrganizationDomain(QLatin1String("liri.io"));
#ifndef QT_NO_SESSIONMANAGER
    app.setFallbackSessionManagementEnabled(false);
#endif
    app.setQuitOnLastWindowClosed(false);

    // Register QML types
    qmlRegisterType<QuickDialog>("io.liri.Portal", 1, 0, "QuickDialog");

    // Register service
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (sessionBus.registerService(QLatin1String("org.freedesktop.impl.portal.desktop.liri"))) {
        DesktopPortal *desktopPortal = new DesktopPortal(&app);
        if (sessionBus.registerObject(QLatin1String("/org/freedesktop/portal/desktop"), desktopPortal, QDBusConnection::ExportAdaptors)) {
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
