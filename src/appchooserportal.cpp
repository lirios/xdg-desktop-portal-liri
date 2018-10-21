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

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "appchooserportal.h"
#include "implementation/appsmodel.h"
#include "logging_p.h"
#include "quickdialog.h"

AppChooserPortal::AppChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint AppChooserPortal::ChooseApplication(const QDBusObjectPath &handle,
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

    auto appsModel = new AppsModel(this);
    appsModel->populate(choices, app_id);

    QQmlApplicationEngine engine(QLatin1String("qrc:/qml/AppChooserDialog.qml"));
    engine.rootContext()->setContextProperty(QLatin1String("appsModel"), appsModel);
    QObject *topLevel = engine.rootObjects().at(0);
    QuickDialog *dialog = qobject_cast<QuickDialog *>(topLevel);
    if (dialog->exec()) {
        results.insert(QLatin1String("choice"), topLevel->property("selectedAppId").toString());
        dialog->close();
        dialog->deleteLater();
        appsModel->deleteLater();
        return 0;
    }

    dialog->deleteLater();
    appsModel->deleteLater();

    return 1;
}
