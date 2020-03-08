// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQmlApplicationEngine>
#include <QStandardPaths>

#include <Qt5AccountsService/UserAccount>
#include <LiriXdg/DesktopFile>

#include "accountportal.h"
#include "logging_p.h"
#include "quickdialog.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Account.xml

AccountPortal::AccountPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

quint32 AccountPortal::GetUserInformation(const QDBusObjectPath &handle,
                                          const QString &app_id,
                                          const QString &parent_window,
                                          const QVariantMap &options,
                                          QVariantMap &results)
{
    qCDebug(lcAccount) << "GetUserInformation called with parameters:";
    qCDebug(lcAccount) << "    handle: " << handle.path();
    qCDebug(lcAccount) << "    app_id: " << app_id;
    qCDebug(lcAccount) << "    parent_window: " << parent_window;
    qCDebug(lcAccount) << "    options: " << options;

    const QString filePath =
            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                   QStringLiteral("%1.desktop").arg(app_id));
    auto *desktop = Liri::DesktopFileCache::getFile(filePath);

    const QString reason = options.value(QStringLiteral("reason")).toString();

    QQmlApplicationEngine engine(QLatin1String("qrc:/qml/UserInfoDialog.qml"));
    QObject *topLevel = engine.rootObjects().at(0);
    QuickDialog *dialog = qobject_cast<QuickDialog *>(topLevel);
    dialog->setProperty("appName", desktop->name());
    dialog->setProperty("reason", reason);
    if (dialog->exec()) {
        auto *account = new QtAccountsService::UserAccount();
        results.insert(QStringLiteral("id"), account->userId());
        results.insert(QStringLiteral("name"), account->realName());
        results.insert(QStringLiteral("image"), account->iconFileName());
        return 0;
    }

    return 1;
}
