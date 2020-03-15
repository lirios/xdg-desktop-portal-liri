// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOCKDOWNPORTAL_H
#define LOCKDOWNPORTAL_H

#include <QDBusAbstractAdaptor>

class LockdownPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
public:
    explicit LockdownPortal(QObject *parent);
};

#endif // LOCKDOWNPORTAL_H
