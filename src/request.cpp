/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#include <QDBusMessage>

#include "request.h"
#include "logging_p.h"

Request::Request(const QString &portalName, const QVariant &data, QObject *parent)
    : QDBusVirtualObject(parent)
    , m_portalName(portalName)
    , m_data(data)
{
}

bool Request::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_UNUSED(connection)

    if (message.type() != QDBusMessage::MessageType::MethodCallMessage)
        return false;

    qCDebug(lcRequest) << "Request:";
    qCDebug(lcRequest) << "    interface: " << message.interface();
    qCDebug(lcRequest) << "    member: " << message.member();
    qCDebug(lcRequest) << "    path: " << message.path();

    return true;
}

QString Request::introspect(const QString &path) const
{
    QString nodes;

    if (path.startsWith(QLatin1String("/org/freedesktop/portal/desktop/request/"))) {
        nodes = QStringLiteral(
            "<interface name=\"org.freedesktop.impl.portal.Request\">"
            "  <method name=\"Close\"/>"
            "</interface>");
    }

    qCDebug(lcRequest) << nodes;

    return nodes;
}
