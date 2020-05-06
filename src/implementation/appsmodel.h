// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QAbstractListModel>

class AppItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(bool preferred MEMBER preferred)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString iconName MEMBER iconName)
public:
    explicit AppItem(QObject *parent = nullptr);

    QString id;
    bool preferred = false;
    QString name;
    QString iconName;
};

class AppsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        PreferredRole,
        NameRole,
        IconNameRole
    };

    explicit AppsModel(QObject *parent = nullptr);

    QHash<int,QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void populate(const QStringList &choices, const QString &defaultAppId = QString());

private:
    QVector<AppItem *> m_apps;
};

#endif // APPSMODEL_H
