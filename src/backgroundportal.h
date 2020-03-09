// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BACKGROUNDPORTAL_H
#define BACKGROUNDPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class BackgroundPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Background")
public:
    enum AutostartFlag {
        None = 0,
        Activatable
    };
    Q_DECLARE_FLAGS(AutostartFlags, AutostartFlag)

    explicit BackgroundPortal(QObject *parent);

public Q_SLOTS:
    QVariantMap GetAppState();
    quint32 NotifyBackground(const QDBusObjectPath &handle,
                             const QString &app_id,
                             const QString &name,
                             QVariantMap &results);
    bool EnableAutostart(const QString &app_id,
                         bool enable,
                         const QStringList &commandline,
                         quint32 flags,
                         QVariantMap &results);

Q_SIGNALS:
    void RunningApplicationsChanged();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BackgroundPortal::AutostartFlags)

#endif // BACKGROUNDPORTAL_H
