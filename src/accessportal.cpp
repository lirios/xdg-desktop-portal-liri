// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQuickItem>

#include "accessportal.h"
#include "logging_p.h"
#include "quickdialog.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Access.xml

AccessPortal::AccessPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

quint32 AccessPortal::AccessDialog(const QDBusObjectPath &handle,
                                   const QString &app_id,
                                   const QString &parent_window,
                                   const QString &title,
                                   const QString &subtitle,
                                   const QString &body,
                                   const QVariantMap &options,
                                   QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcAccess) << "AccessDialog called with parameters:";
    qCDebug(lcAccess) << "    handle: " << handle.path();
    qCDebug(lcAccess) << "    app_id: " << app_id;
    qCDebug(lcAccess) << "    parent_window: " << parent_window;
    qCDebug(lcAccess) << "    title: " << title;
    qCDebug(lcAccess) << "    subtitle: " << subtitle;
    qCDebug(lcAccess) << "    body: " << body;
    qCDebug(lcAccess) << "    options: " << options;

    const bool modal = options.value(QStringLiteral("modal"), true).toBool();
    const QString grantLabel = options.value(QStringLiteral("grant_label"), true).toString();
    const QString denyLabel = options.value(QStringLiteral("deny_label"), true).toString();

    // TODO: choices

    auto *dialog = new QuickDialog();
    dialog->setTitle(title);
    dialog->setModal(modal);
    dialog->rootObject()->setProperty("title", title);
    dialog->rootObject()->setProperty("subtitle", subtitle);
    dialog->rootObject()->setProperty("body", body);
    dialog->rootObject()->setProperty("modal", modal);
    dialog->setSource(QUrl(QLatin1String("qrc:/qml/AccessDialog.qml")));
    if (!grantLabel.isEmpty())
        dialog->rootObject()->setProperty("grantLabel", grantLabel);
    if (!denyLabel.isEmpty())
        dialog->rootObject()->setProperty("denyLabel", denyLabel);
    if (dialog->exec()) {
        dialog->deleteLater();
        return 0;
    }

    dialog->deleteLater();

    return 1;
}
