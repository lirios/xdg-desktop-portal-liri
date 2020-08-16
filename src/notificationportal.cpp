// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2016 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>

#include "notificationportal.h"
#include "logging_p.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Notification.xml

static Liri::Notification::Urgency convertUrgency(const QString &urgency)
{
    if (urgency == QLatin1String("low"))
        return Liri::Notification::UrgencyLow;
    else if (urgency == QLatin1String("normal"))
        return Liri::Notification::UrgencyNormal;
    else if (urgency == QLatin1String("high"))
        return Liri::Notification::UrgencyCritical;
    else if (urgency == QLatin1String("urgent"))
        return Liri::Notification::UrgencyCritical;
    Q_UNREACHABLE();
}

NotificationPortal::NotificationPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

void NotificationPortal::AddNotification(const QString &app_id,
                                         const QString &id,
                                         const QVariantMap &notification)
{
    qCDebug(lcNotification) << "AddNotification called with parameters:";
    qCDebug(lcNotification) << "    app_id: " << app_id;
    qCDebug(lcNotification) << "    id: " << id;
    qCDebug(lcNotification) << "    notification: " << notification;

    bool numericIdOk = false;
    int numericId = id.toInt(&numericIdOk);

    auto notify = new Liri::Notification(this);
    connect(notify, &Liri::Notification::actionInvoked,
            this, &NotificationPortal::notificationActionInvoked);
    connect(notify, &Liri::Notification::closed,
            this, &NotificationPortal::notificationClosed);
    notify->setHint(QLatin1String("desktop-entry"), app_id);
    if (numericIdOk)
        notify->setReplacesId(numericId);
    if (notification.contains(QLatin1String("title")))
        notify->setSummary(notification.value(QLatin1String("title")).toString());
    if (notification.contains(QLatin1String("body")))
        notify->setBody(notification.value(QLatin1String("body")).toString());
    if (notification.contains(QLatin1String("icon")))
        notify->setApplicationIcon(notification.value(QLatin1String("icon")).toString());
    if (notification.contains(QLatin1String("priority")))
        notify->setUrgency(convertUrgency(notification.value(QLatin1String("priority")).toString()));
    if (notification.contains(QLatin1String("default-action")))
        notify->setDefaultAction(notification.value(QLatin1String("default-action")).toString());
    if (notification.contains(QLatin1String("default-action-target")))
        notify->setDefaultActionTarget(notification.value(QLatin1String("default-action-target")).toString());
    if (notification.contains(QLatin1String("buttons"))) {
        QList<QVariantMap> buttons;
        QDBusArgument dbusArgument = notification.value(QLatin1String("buttons")).value<QDBusArgument>();
        while (!dbusArgument.atEnd())
            dbusArgument >> buttons;

        QStringList actions;
        for (const QVariantMap &button : buttons)
            actions << button.value(QLatin1String("label")).toString();

        if (!actions.isEmpty())
            notify->setActions(actions);
    }
    if (notification.contains(QLatin1String("title")))
        notify->setSummary(notification.value(QLatin1String("title")).toString());
    m_notifications.insert(QStringLiteral("%1:%2").arg(app_id, id), notify);
    notify->send();
}

void NotificationPortal::RemoveNotification(const QString &app_id,
                                            const QString &id)
{
    qCDebug(lcNotification) << "RemoveNotification called with parameters:";
    qCDebug(lcNotification) << "    app_id: " << app_id;
    qCDebug(lcNotification) << "    id: " << id;

    Liri::Notification *notify = m_notifications.take(QStringLiteral("%1:%2").arg(app_id, id));
    if (notify) {
        notify->close();
        notify->deleteLater();
    }
}

QString NotificationPortal::findKey(Liri::Notification *notify, QString &appId, QString &id) const
{
    QString key = m_notifications.key(notify, QString());
    if (key.isEmpty())
        return QString();
    const QStringList elements = key.split(QLatin1Char(':'));
    if (elements.size() != 2)
        return QString();
    appId = elements.at(0);
    id = elements.at(1);
    return key;
}

void NotificationPortal::notificationActionInvoked(const QString &action)
{
    auto notify = qobject_cast<Liri::Notification *>(sender());
    if (!notify)
        return;

    QString appId, id;
    QString key = findKey(notify, appId, id);
    if (key.isEmpty())
        return;

    qCDebug(lcNotification) << "Notification action invoked:";
    qCDebug(lcNotification) << "    app_id: " << appId;
    qCDebug(lcNotification) << "    id: " << id;
    qCDebug(lcNotification) << "    action: " << action;

    QDBusMessage message =
            QDBusMessage::createSignal(QLatin1String("/org/freedesktop/portal/desktop"),
                                       QLatin1String("org.freedesktop.impl.portal.Notification"),
                                       QLatin1String("ActionInvoked"));
    message << appId << id << action << QVariantList();
    QDBusConnection::sessionBus().send(message);
}

void NotificationPortal::notificationClosed(Liri::Notification::CloseReason reason)
{
    Q_UNUSED(reason)

    auto notify = qobject_cast<Liri::Notification *>(sender());
    if (!notify)
        return;

    QString appId, id;
    QString key = findKey(notify, appId, id);
    if (key.isEmpty())
        return;

    qCDebug(lcNotification) << "Notification closed:";
    qCDebug(lcNotification) << "    app_id: " << appId;
    qCDebug(lcNotification) << "    id: " << id;

    m_notifications.remove(key);
    notify->deleteLater();
}
