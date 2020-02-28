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

#include <QGuiApplication>

#include "screencastportal.h"
#include "session.h"
#include "logging_p.h"
#include "waylandintegration.h"

ScreenCastPortal::ScreenCastPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint ScreenCastPortal::version() const
{
    return 1;
}

uint ScreenCastPortal::AvailableSourceTypes() const
{
    return Monitor;
}

uint ScreenCastPortal::CreateSession(const QDBusObjectPath &handle,
                                     const QDBusObjectPath &session_handle,
                                     const QString &app_id,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    qCDebug(lcScreenCast) << "CreateSession called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    options: " << options;

    if (m_lastSession) {
        qCWarning(lcScreenCast, "Another screen cast session is running");
        return 2;
    }

    auto *session = Session::createSession(Session::ScreenCast, app_id, session_handle.path(), this);
    if (!session)
        return 2;

    m_lastSession = session;

    return 0;
}

uint ScreenCastPortal::SelectSources(const QDBusObjectPath &handle,
                                     const QDBusObjectPath &session_handle,
                                     const QString &app_id,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcScreenCast) << "SelectSource called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    options: " << options;

    SourceType types = Monitor;

    auto *session = qobject_cast<ScreenCastSession*>(Session::getSession(session_handle.path()));
    if (!session) {
        qCWarning(lcScreenCast, "Cannot select sources for screen cast: session %s doesn't exist",
                  qPrintable(session_handle.path()));
        return 2;
    }

    if (options.contains(QStringLiteral("multiple")))
        session->setHasMultipleSources(options.value(QStringLiteral("multiple")).toBool());
    if (options.contains(QStringLiteral("types")))
        types = static_cast<SourceType>(options.value(QStringLiteral("types")).toUInt());

    if (types == Window) {
        qCWarning(lcScreenCast, "Screen cast of a window is not implemented");
        return 2;
    }

    // TODO: Remote desktop

    return 0;
}

uint ScreenCastPortal::Start(const QDBusObjectPath &handle,
                             const QDBusObjectPath &session_handle,
                             const QString &app_id,
                             const QString &parent_window,
                             const QVariantMap &options,
                             QVariantMap &results)
{
    qCDebug(lcScreenCast) << "Start called with parameters:";
    qCDebug(lcScreenCast) << "    handle: " << handle.path();
    qCDebug(lcScreenCast) << "    session_handle: " << session_handle.path();
    qCDebug(lcScreenCast) << "    app_id: " << app_id;
    qCDebug(lcScreenCast) << "    parent_window: " << parent_window;
    qCDebug(lcScreenCast) << "    options: " << options;

    auto *session = qobject_cast<ScreenCastSession *>(Session::getSession(session_handle.path()));
    if (!session) {
        qCWarning(lcScreenCast, "Cannot start screen cast: session %s doesn't exist",
                  qPrintable(session_handle.path()));
        return 2;
    }

    // TODO: Choose the screen from a window
    QScreen *selectedScreen = QGuiApplication::primaryScreen();

    // Stop streaming when session is closed
    connect(session, &Session::closed, this, [this, selectedScreen] {
        m_lastSession = nullptr;
        WaylandIntegration::instance()->stopStreaming(selectedScreen);
    });

    // Start streaming
    if (!WaylandIntegration::instance()->startStreaming(selectedScreen))
        return 2;

    // Return the streams
    QVariant streams = WaylandIntegration::instance()->streams();
    if (!streams.isValid()) {
        qCWarning(lcScreenCast, "PipeWire stream is not ready");
        return 2;
    }
    results.insert(QStringLiteral("streams"), streams);

    return 0;
}
