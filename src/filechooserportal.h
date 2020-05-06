// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2016-2018 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FILECHOOSERPORTAL_H
#define FILECHOOSERPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class FileChooserPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.FileChooser")
public:
    typedef struct {
        quint32 type;
        QString filterString;
    } Filter;
    typedef QVector<Filter> Filters;

    typedef struct {
        QString userVisibleName;
        Filters filters;
    } FilterList;
    typedef QVector<FilterList> FilterListList;

    explicit FileChooserPortal(QObject *parent = nullptr);

public Q_SLOTS:
    quint32 OpenFile(const QDBusObjectPath &handle,
                     const QString &app_id,
                     const QString &parent_window,
                     const QString &title,
                     const QVariantMap &options,
                     QVariantMap &results);
    quint32 SaveFile(const QDBusObjectPath &handle,
                     const QString &app_id,
                     const QString &parent_window,
                     const QString &title,
                     const QVariantMap &options,
                     QVariantMap &results);
    quint32 SaveFiles(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results);
};

#endif // FILECHOOSERPORTAL_H
