// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2018-2019 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusContext>
#include <QDBusMessage>
#include <QDBusMetaType>

#include <Qt5GSettings/Qt5GSettings>

#include "logging_p.h"
#include "settingsportal.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Settings.xml

Q_DECLARE_METATYPE(VariantMapMap)

QDBusArgument &operator<<(QDBusArgument &argument, const VariantMapMap &map)
{
    argument.beginMap(QVariant::String, QVariant::Map);

    QMapIterator<QString, QVariantMap> i(map);
    while (i.hasNext()) {
        i.next();
        argument.beginMapEntry();
        argument << i .key() << i.value();
        argument.endMapEntry();
    }

    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, VariantMapMap &map)
{
    argument.beginMap();
    map.clear();

    while (!argument.atEnd()) {
        QString key;
        QVariantMap value;

        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        map.insert(key, value);
    }

    argument.endMap();
    return argument;
}


static bool schemaMatches(const QString &schema, const QStringList &patterns)
{
    for (const auto &pattern : patterns) {
        if (pattern.isEmpty() || pattern == schema)
            return true;

        if (pattern.endsWith(QLatin1Char('*')) && schema.startsWith(pattern.left(pattern.length() - 1)))
            return true;
    }

    return false;
}


SettingsPortal::SettingsPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<VariantMapMap>();

    for (const auto &schema : QtGSettings::QGSettings::schemas()) {
        auto *settings = new QtGSettings::QGSettings(schema, QString(), this);
        connect(settings, &QtGSettings::QGSettings::settingChanged,
                this, &SettingsPortal::handleSettingChanged);
        m_settings.insert(schema, settings);
    }
}

SettingsPortal::~SettingsPortal()
{
    qDeleteAll(m_settings);
}

quint32 SettingsPortal::version() const
{
    return 1;
}

VariantMapMap SettingsPortal::ReadAll(const QStringList &schemas)
{
    qCDebug(lcSettings) << "ReadAll called with parameters:";
    qCDebug(lcSettings) << "    namespaces: " << schemas;

    VariantMapMap result;

    for (const auto &schema : QtGSettings::QGSettings::schemas()) {
        if (!schemaMatches(schema, schemas))
            continue;

        auto *settings = getSettings(schema);

        QVariantMap values;
        for (const auto &key : settings->keys())
            values.insert(key, settings->value(key));
        result.insert(schema, values);
    }

    return result;
}

QDBusVariant SettingsPortal::Read(const QString &schema, const QString &key)
{
    qCDebug(lcSettings) << "Read called with parameters:";
    qCDebug(lcSettings) << "    namespace: " << schema;
    qCDebug(lcSettings) << "    key: " << key;

    if (!parent()) {
        qCWarning(lcSettings, "Failed to get D-Bus context");
        return QDBusVariant();
    }

    auto *context = reinterpret_cast<QDBusContext *>(parent()->qt_metacast("QDBusContext"));
    if (!context) {
        qCWarning(lcSettings, "Failed to get D-Bus context");
        return QDBusVariant();
    }

    if (!schema.startsWith(QStringLiteral("io.liri.desktop"))) {
        qCWarning(lcSettings, "Namespace \"%s\" is not supported",
                  qPrintable(schema));

        auto message = context->message();
        auto reply = message.createErrorReply(
                    QDBusError::UnknownProperty,
                    QStringLiteral("Namespace %1 is not supported").arg(schema));
        QDBusConnection::sessionBus().send(reply);
        return QDBusVariant();
    }

    auto *settings = getSettings(schema);
    auto value = settings->value(key);
    if (value.isNull()) {
        qCWarning(lcSettings, "Property \"%s\" from \"%s\" doesn't exist",
                  qPrintable(key), qPrintable(schema));

        auto message = context->message();
        auto reply = message.createErrorReply(
                    QDBusError::UnknownProperty,
                    QStringLiteral("Property %1 from %2 doesn't exist").arg(key, schema));
        QDBusConnection::sessionBus().send(reply);
        return QDBusVariant();
    }

    QDBusVariant dbusValue;
    dbusValue.setVariant(value);
    return dbusValue;
}

QtGSettings::QGSettings *SettingsPortal::getSettings(const QString &schema)
{
    if (m_settings.contains(schema)) {
        return m_settings[schema];
    } else {
        auto *settings = new QtGSettings::QGSettings(schema);
        connect(settings, &QtGSettings::QGSettings::settingChanged,
                this, &SettingsPortal::handleSettingChanged);
        m_settings.insert(schema, settings);
        return settings;
    }
}

void SettingsPortal::handleSettingChanged(const QString &key)
{
    auto *settings = qobject_cast<QtGSettings::QGSettings *>(sender());

    QDBusVariant value;
    value.setVariant(settings->value(key));

    Q_EMIT SettingChanged(settings->id(), key, value);
}
