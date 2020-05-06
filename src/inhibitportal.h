// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INHIBITPORTAL_H
#define INHIBITPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class InhibitPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Inhibit")
public:
    explicit InhibitPortal(QObject *parent);

public Q_SLOTS:
    void Inhibit(const QDBusObjectPath &handle,
                 const QString &app_id,
                 const QString &window,
                 uint flags,
                 const QVariantMap &options);
};

#endif // INHIBITPORTAL_H
