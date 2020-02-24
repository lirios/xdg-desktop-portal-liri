/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#include <QDBusConnection>
#include <QDBusMessage>

#include "session.h"
#include "logging_p.h"

static QMap<QString, Session *> sessionList;

// Session

Session::Session(const QString &appId, const QString &path, QObject *parent)
    : QDBusVirtualObject(parent)
    , m_appId(appId)
    , m_path(path)
{
}

bool Session::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    if (message.path() != m_path)
        return false;

    if (message.type() != QDBusMessage::MessageType::MethodCallMessage)
        return false;

    qCDebug(lcSession) << "Session:";
    qCDebug(lcSession) << "    interface: " << message.interface();
    qCDebug(lcSession) << "    member: " << message.member();
    qCDebug(lcSession) << "    path: " << message.path();

    if (message.interface() == QLatin1String("org.freedesktop.impl.portal.Session")) {
        if (message.member() == QLatin1String("Close")) {
            emit closed();

            auto reply = message.createReply();
            return connection.send(reply);
        }
    } else if (message.interface() == QLatin1String("org.freedesktop.DBus.Properties")) {
        if (message.member() == QLatin1String("Get")) {
            if (message.arguments().count() == 2) {
                const QString interface = message.arguments().at(0).toString();
                const QString property = message.arguments().at(1).toString();

                if (interface == QLatin1String("org.freedesktop.impl.portal.Session") &&
                        property == QLatin1String("version")) {
                    QList<QVariant> arguments;
                    arguments << 1;

                    QDBusMessage reply = message.createReply();
                    reply.setArguments(arguments);
                    return connection.send(reply);
                }
            }
        }
    }

    return false;
}

QString Session::introspect(const QString &path) const
{
    QString nodes;

    if (path.startsWith(QLatin1String("/org/freedesktop/portal/desktop/session/"))) {
        nodes = QStringLiteral(
            "<interface name=\"org.freedesktop.impl.portal.Session\">"
            "  <method name=\"Close\"/>"
            "  <signal name=\"Closed\"/>"
            "  <property name=\"version\" type=\"u\" access=\"read\"/>"
            "</interface>");
    }

    qCDebug(lcSession) << nodes;

    return nodes;
}

bool Session::close()
{
    auto reply = QDBusMessage::createSignal(
                m_path,
                QStringLiteral("org.freedesktop.impl.portal.Session"),
                QStringLiteral("Closed"));
    return QDBusConnection::sessionBus().send(reply);
}

Session *Session::createSession(Session::Type type, const QString &appId, const QString &path, QObject *parent)
{
    auto bus = QDBusConnection::sessionBus();

    Session *session = nullptr;

    if (type == ScreenCast)
        session = new ScreenCastSession(appId, path, parent);

    Q_ASSERT(session);

    if (bus.registerVirtualObject(path, session, QDBusConnection::VirtualObjectRegisterOption::SubPath)) {
        connect(session, &Session::closed, [session, path]() {
            sessionList.remove(path);

            QDBusConnection::sessionBus().unregisterObject(path);
            session->deleteLater();
        });

        sessionList.insert(path, session);
        return session;
    }

    qCWarning(lcSession, "Failed to register session object \"%s\": %s",
              qPrintable(path), qPrintable(bus.lastError().message()));
    session->deleteLater();

    return nullptr;
}

Session *Session::getSession(const QString &sessionHandle)
{
    return sessionList.value(sessionHandle);
}

// ScreenCastSession

ScreenCastSession::ScreenCastSession(const QString &appId, const QString &path, QObject *parent)
    : Session(appId, path, parent)
{
}

Session::Type ScreenCastSession::type() const
{
    return Session::ScreenCast;
}

bool ScreenCastSession::hasMultipleSources() const
{
    return m_hasMultipleSources;
}

void ScreenCastSession::setHasMultipleSources(bool value)
{
    if (m_hasMultipleSources == value)
        return;

    m_hasMultipleSources = value;
    emit hasMultipleSourcesChanged();
}
