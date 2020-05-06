// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2020 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "backgroundportal.h"
#include "logging_p.h"
#include "waylandintegration.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Background.xml

static QString quoteExecPart(const QString &part)
{
    const QChar c = part[0];

    if (!c.isNumber() &&
            !(c == QLatin1Char('-') || c == QLatin1Char('/') || c == QLatin1Char('~') ||
              c == QLatin1Char(':') || c == QLatin1Char('.') || c == QLatin1Char('_') ||
              c == QLatin1Char('=') || c == QLatin1Char('@')))
        return QStringLiteral("'%1'").arg(part);
    return part;
}

static QString constructExec(const QStringList &commandLine)
{
    QString exec;

    for (const auto &part : commandLine) {
        if (commandLine.first() != part)
            exec.append(QLatin1Char(' '));
        exec.append(quoteExecPart(part));
    }

    return exec;
}


BackgroundPortal::BackgroundPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    connect(WaylandIntegration::instance(), &WaylandIntegration::toplevelsChanged,
            this, &BackgroundPortal::RunningApplicationsChanged);
}

QVariantMap BackgroundPortal::GetAppState()
{
    QVariantMap apps;

    auto toplevels = WaylandIntegration::instance()->toplevels();
    for (auto *toplevel : qAsConst(toplevels))
        apps[toplevel->appId()] = toplevel->isActive() ? 2 : 1;

    return apps;
}

quint32 BackgroundPortal::NotifyBackground(const QDBusObjectPath &handle,
                                           const QString &app_id,
                                           const QString &name,
                                           QVariantMap &results)
{
    qCDebug(lcBackground) << "NotifyBackground called with parameters:";
    qCDebug(lcBackground) << "    handle: " << handle.path();
    qCDebug(lcBackground) << "    app_id: " << app_id;
    qCDebug(lcBackground) << "    name: " << name;

    results.insert(QStringLiteral("result"), 2);
    return 0;
}

bool BackgroundPortal::EnableAutostart(const QString &app_id,
                                       bool enable,
                                       const QStringList &commandline,
                                       quint32 flags)
{
    qCDebug(lcBackground) << "EnableAutostart called with parameters:";
    qCDebug(lcBackground) << "    app_id: " << app_id;
    qCDebug(lcBackground) << "    enable: " << enable;
    qCDebug(lcBackground) << "    commandline: " << commandline;
    qCDebug(lcBackground) << "    flags: " << flags;

    const auto fileName = app_id + QStringLiteral(".desktop");
    const auto directory =
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
            QStringLiteral("/autostart/");
    const auto filePath = directory + fileName;
    const auto autostartFlags = static_cast<AutostartFlags>(flags);

    if (enable) {
        QDir dir(directory);
        if (!dir.mkpath(dir.absolutePath())) {
            qCWarning(lcBackground, "Failed to create autostart directory");
            return false;
        }

        // Find the original desktop entry to get the localized name
        auto appDesktopFilePath =
                QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                       app_id + QStringLiteral(".desktop"));
        auto *appDesktop = Liri::DesktopFileCache::getFile(appDesktopFilePath);

        auto *desktop = new Liri::DesktopFile();

        desktop->beginGroup(QStringLiteral("Desktop Entry"));
        desktop->setValue(QStringLiteral("Type"), QStringLiteral("Application"));
        if (appDesktop) {
            desktop->setValue(QStringLiteral("Name"), appDesktop->value(QStringLiteral("Name")));
            desktop->setValue(QStringLiteral("GenericName"), appDesktop->value(QStringLiteral("GenericName")));
            desktop->setValue(QStringLiteral("Comment"), appDesktop->value(QStringLiteral("Comment")));
            desktop->setLocalizedValue(QStringLiteral("Name"), appDesktop->name());
            desktop->setLocalizedValue(QStringLiteral("GenericName"), appDesktop->genericName());
            desktop->setLocalizedValue(QStringLiteral("Comment"), appDesktop->comment());
        } else {
            desktop->setValue(QStringLiteral("Name"), app_id);
        }
        desktop->setValue(QStringLiteral("Exec"), constructExec(commandline));
        if (autostartFlags.testFlag(AutostartFlag::Activatable))
            desktop->setValue(QStringLiteral("DBusActivatable"), true);
        desktop->setValue(QStringLiteral("X-Flatpak"), app_id);
        desktop->save(filePath);
    } else {
        QFile file(filePath);
        if (!file.remove()) {
            qCWarning(lcBackground,
                      "Failed to delete \"%s\" in order to disable autostart for \"%s\": %s",
                      qPrintable(filePath),
                      qPrintable(app_id),
                      qPrintable(file.errorString()));
            return false;
        }
    }

    return true;
}
