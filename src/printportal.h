// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PRINTPORTAL_H
#define PRINTPORTAL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusUnixFileDescriptor>

class QPrinter;

class PrintPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Print")
public:
    explicit PrintPortal(QObject *parent);

public Q_SLOTS:
    quint32 Print(const QDBusObjectPath &handle,
                  const QString &app_id,
                  const QString &parent_window,
                  const QString &title,
                  const QDBusUnixFileDescriptor &fd,
                  const QVariantMap &options,
                  QVariantMap &results);
    quint32 PreparePrint(const QDBusObjectPath &handle,
                         const QString &app_id,
                         const QString &parent_window,
                         const QString &title,
                         const QVariantMap &settings,
                         const QVariantMap &page_setup,
                         const QVariantMap &options,
                         QVariantMap &results);

private:
    QMap<quint32, QPrinter *> m_printers;
};

#endif // PRINTPORTAL_H
