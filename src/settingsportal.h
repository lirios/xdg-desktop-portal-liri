// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SETTINGSPORTAL_H
#define SETTINGSPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

typedef QMap<QString, QVariantMap> VariantMapMap;

namespace QtGSettings {
class QGSettings;
}

class SettingsPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Settings")
public:
    explicit SettingsPortal(QObject *parent);
    ~SettingsPortal();

    quint32 version() const;

Q_SIGNALS:
    void SettingChanged(const QString &schema,
                        const QString &key,
                        const QDBusVariant &value);

public Q_SLOTS:
    VariantMapMap ReadAll(const QStringList &schemas);
    QDBusVariant Read(const QString &schema,
                      const QString &key);

private:
    QMap<QString, QtGSettings::QGSettings *> m_settings;

    QtGSettings::QGSettings *getSettings(const QString &schema);

private Q_SLOTS:
    void handleSettingChanged(const QString &key);
};

#endif // SETTINGSPORTAL_H
