/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

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

    void populate(const QStringList &choices, const QString &defaultAppId);

private:
    QVector<AppItem *> m_apps;
};

#endif // APPSMODEL_H
