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

#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "appsmodel.h"
#include "logging_p.h"

AppItem::AppItem(QObject *parent)
    : QObject(parent)
{
}

AppsModel::AppsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> AppsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, QByteArrayLiteral("id"));
    roles.insert(PreferredRole, QByteArrayLiteral("preferred"));
    roles.insert(NameRole, QByteArrayLiteral("name"));
    roles.insert(IconNameRole, QByteArrayLiteral("iconName"));
    return roles;
}

int AppsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_apps.size();
}

QVariant AppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() > m_apps.size() - 1)
        return QVariant();

    AppItem *item = m_apps.at(index.row());

    switch (role) {
    case IdRole:
        return item->id;
    case PreferredRole:
        return item->preferred;
    case Qt::DisplayRole:
    case NameRole:
        return item->name;
    case IconNameRole:
        return item->iconName;
    }

    return QVariant();
}

void AppsModel::populate(const QStringList &choices, const QString &defaultAppId)
{
    beginResetModel();
    qDeleteAll(m_apps);
    endResetModel();

    for (const auto &appId : choices) {
        const QString desktopFileBaseName = appId + QLatin1String(".desktop");
        const QStringList desktopFilesLocations =
                QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation,
                                          desktopFileBaseName, QStandardPaths::LocateFile);

        for (const auto &desktopFileName : desktopFilesLocations) {
            auto *desktop = Liri::DesktopFileCache::getFile(desktopFileName);
            if (!desktop) {
                qCWarning(lcAppChooser) << "Unable to open" << desktopFileName;
                continue;
            }

            // Localized name
            QString name;
            if (desktop->contains(QStringLiteral("X-GNOME-FullName")))
                name = desktop->localizedValue(QStringLiteral("X-GNOME-FullName")).toString();
            if (name.isEmpty())
                name = desktop->name();

            beginInsertRows(QModelIndex(), m_apps.size(), m_apps.size());
            auto appItem = new AppItem(this);
            appItem->id = appId;
            appItem->preferred = appId == defaultAppId;
            appItem->name = name;
            appItem->iconName = desktop->iconName();
            m_apps.append(appItem);
            endInsertRows();
        }
    }
}
