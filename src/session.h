/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef SESSION_H
#define SESSION_H

#include <QDBusVirtualObject>

class Session : public QDBusVirtualObject
{
    Q_OBJECT
public:
    enum Type {
        ScreenCast,
        RemoteDesktop
    };
    Q_ENUM(Type)

    explicit Session(const QString &appId = QString(),
                     const QString &path = QString(),
                     QObject *parent = nullptr);

    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection) override;
    QString introspect(const QString &path) const override;

    bool close();

    virtual Type type() const = 0;

    static Session *createSession(Type type, const QString &appId, const QString &path, QObject *parent);
    static Session *getSession(const QString &sessionHandle);

Q_SIGNALS:
    void closed();

private:
    QString m_appId;
    QString m_path;
};

class ScreenCastSession : public Session
{
    Q_OBJECT
    Q_PROPERTY(bool hasMultipleSources READ hasMultipleSources WRITE setHasMultipleSources NOTIFY hasMultipleSourcesChanged)
public:
    explicit ScreenCastSession(const QString &appId = QString(),
                               const QString &path = QString(),
                               QObject *parent = nullptr);

    Type type() const;

    bool hasMultipleSources() const;
    void setHasMultipleSources(bool value);

Q_SIGNALS:
    void hasMultipleSourcesChanged();

private:
    bool m_hasMultipleSources = false;
};

#endif // SESSION_H
