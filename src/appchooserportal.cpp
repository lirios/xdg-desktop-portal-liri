// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMimeDatabase>
#include <QMimeType>
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

    const auto lastChoice = options.value(QStringLiteral("last_choice")).toString();
    const auto fileName = options.value(QStringLiteral("filename")).toString();
    const auto uri = options.value(QStringLiteral("uri")).toString();
    const auto contentType = options.value(QStringLiteral("content_type")).toString();

    QString fileType;
    if (!contentType.isEmpty()) {
        QMimeDatabase mimeDb;
        for (const auto &mimeType : mimeDb.allMimeTypes()) {
            if (mimeType.inherits(contentType)) {
                fileType = mimeType.name();
                break;
            }
        }
    }

    auto *model = new AppsModel(this);
    m_models[handle.path()] = model;
    m_lastChoices[handle.path()] = lastChoice;

    model->populate(choices, lastChoice);

    auto *dialog = new QuickDialog(QUrl(QLatin1String("qrc:/qml/AppChooserDialog.qml")));
    dialog->rootContext()->setContextProperty(QLatin1String("appsModel"), model);
    dialog->rootContext()->setContextProperty(QStringLiteral("fileName"), fileName);
    dialog->rootContext()->setContextProperty(QStringLiteral("uri"), uri);
    dialog->rootContext()->setContextProperty(QStringLiteral("contentType"), contentType);
    dialog->rootContext()->setContextProperty(QStringLiteral("fileType"), fileType);

    bool result = dialog->exec() == 0;

    if (result)
        results.insert(QLatin1String("choice"), dialog->rootObject()->property("selectedAppId"));

    m_models.remove(handle.path());
    m_lastChoices.remove(handle.path());
    dialog->deleteLater();
    model->deleteLater();

    return result ? 0 : 1;
}

void AppChooserPortal::UpdateChoices(const QDBusObjectPath &handle,
                                     const QStringList &choices)
{
    qCDebug(lcAppChooser) << "UpdateChoices called with parameters:";
    qCDebug(lcAppChooser) << "    handle: " << handle.path();
    qCDebug(lcAppChooser) << "    choices: " << choices;

    if (m_models.contains(handle.path())) {
        auto lastChoice = m_lastChoices.value(handle.path());
        m_models[handle.path()]->populate(choices, lastChoice);
    }
}
