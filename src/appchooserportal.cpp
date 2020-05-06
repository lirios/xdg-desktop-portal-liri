// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQmlContext>
#include <QQuickItem>

#include "appchooserportal.h"
#include "implementation/appsmodel.h"
#include "logging_p.h"
#include "quickdialog.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.AppChooser.xml

AppChooserPortal::AppChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_appsModel(new AppsModel(this))
{
}

quint32 AppChooserPortal::ChooseApplication(const QDBusObjectPath &handle,
                                            const QString &app_id,
                                            const QString &parent_window,
                                            const QStringList &choices,
                                            const QVariantMap &options,
                                            QVariantMap &results)
{
    qCDebug(lcAppChooser) << "ChooseApplication called with parameters:";
    qCDebug(lcAppChooser) << "    handle: " << handle.path();
    qCDebug(lcAppChooser) << "    app_id: " << app_id;
    qCDebug(lcAppChooser) << "    parent_window: " << parent_window;
    qCDebug(lcAppChooser) << "    choices: " << choices;
    qCDebug(lcAppChooser) << "    options: " << options;

    auto *dialog = new QuickDialog(QUrl(QLatin1String("qrc:/qml/AppChooserDialog.qml")));
    dialog->rootContext()->setContextProperty(QLatin1String("appsModel"), m_appsModel);
    if (dialog->exec()) {
        results.insert(QLatin1String("choice"), dialog->rootObject()->property("selectedAppId").toString());
        dialog->deleteLater();
        return 0;
    }

    dialog->deleteLater();
    return 1;
}

void AppChooserPortal::UpdateChoices(const QDBusObjectPath &handle,
                                     const QStringList &choices)
{
    qCDebug(lcAppChooser) << "UpdateChoices called with parameters:";
    qCDebug(lcAppChooser) << "    handle: " << handle.path();
    qCDebug(lcAppChooser) << "    choices: " << choices;

    m_appsModel->populate(choices);
}
