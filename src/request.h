/****************************************************************************
 * SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef REQUEST_H
#define REQUEST_H

#include <QDBusVirtualObject>

class Request : public QDBusVirtualObject
{
    Q_OBJECT
public:
    explicit Request(const QString &portalName = QString(),
                     const QVariant &data = QVariant(),
                     QObject *parent = nullptr);

    bool handleMessage(const QDBusMessage &message,
                       const QDBusConnection &connection) override;
    QString introspect(const QString &path) const override;

private:
    QString m_portalName;
    QVariant m_data;
};

#endif // REQUEST_H
