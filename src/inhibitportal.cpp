// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
