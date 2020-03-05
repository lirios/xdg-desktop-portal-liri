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

#include <QDesktopServices>
#include <QUrl>

#include "emailportal.h"
#include "logging_p.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Email.xml

EmailPortal::EmailPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

uint EmailPortal::ComposeEmail(const QDBusObjectPath &handle,
                               const QString &app_id,
                               const QString &window,
                               const QVariantMap &options,
                               QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcEmail) << "ComposeEmail called with parameters:";
    qCDebug(lcEmail) << "    handle: " << handle.path();
    qCDebug(lcEmail) << "    app_id: " << app_id;
    qCDebug(lcEmail) << "    window: " << window;
    qCDebug(lcEmail) << "    options: " << options;

    const QString address = options.value(QStringLiteral("address")).toString();
    const QString subject = options.value(QStringLiteral("subject")).toString();
    const QString body = options.value(QStringLiteral("body")).toString();
    const QStringList attachments = options.value(QStringLiteral("attachments")).toStringList();

    QString attachmentString;
    for (const QString &attachment : attachments)
        attachmentString += QStringLiteral("&attachment=%1").arg(attachment);

    const QString mailtoString = QStringLiteral("mailto:%1?subject=%2&body=%3%4")
            .arg(address, subject, body, attachmentString);
    return QDesktopServices::openUrl(QUrl(mailtoString));
}
