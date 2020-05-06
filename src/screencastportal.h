// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2018 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENCASTPORTAL_H
#define SCREENCASTPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class Session;

class ScreenCastPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.ScreenCast")
    Q_PROPERTY(uint version READ version CONSTANT)
    Q_PROPERTY(uint AvailableSourceTypes READ AvailableSourceTypes CONSTANT)
public:
    enum SourceType {
        Any = 0,
        Monitor,
        Window
    };
    Q_ENUM(SourceType)

    explicit ScreenCastPortal(QObject *parent);

    uint version() const;
    uint AvailableSourceTypes() const;

public Q_SLOTS:
    uint CreateSession(const QDBusObjectPath &handle,
                       const QDBusObjectPath &session_handle,
                       const QString &app_id,
                       const QVariantMap &options,
                       QVariantMap &results);
    uint SelectSources(const QDBusObjectPath &handle,
                       const QDBusObjectPath &session_handle,
                       const QString &app_id,
                       const QVariantMap &options,
                       QVariantMap &results);
    uint Start(const QDBusObjectPath &handle,
               const QDBusObjectPath &session_handle,
               const QString &app_id,
               const QString &parent_window,
               const QVariantMap &options,
               QVariantMap &results);

private:
    Session *m_lastSession = nullptr;
};

#endif // SCREENCASTPORTAL_H
