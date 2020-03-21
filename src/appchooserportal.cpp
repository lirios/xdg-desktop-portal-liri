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
