// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

    // HiDPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

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
