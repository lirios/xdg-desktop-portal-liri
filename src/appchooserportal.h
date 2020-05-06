// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPCHOOSERPORTAL_H
#define APPCHOOSERPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class AppsModel;

class AppChooserPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.AppChooser")
public:
    explicit AppChooserPortal(QObject *parent);

public Q_SLOTS:
    quint32 ChooseApplication(const QDBusObjectPath &handle,
                              const QString &app_id,
                              const QString &parent_window,
                              const QStringList &choices,
                              const QVariantMap &options,
                              QVariantMap &results);
    void UpdateChoices(const QDBusObjectPath &handle,
                       const QStringList &choices);

private:
    AppsModel *m_appsModel = nullptr;
};

#endif // APPCHOOSERPORTAL_H
