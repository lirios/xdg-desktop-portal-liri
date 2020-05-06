// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

quint32 EmailPortal::ComposeEmail(const QDBusObjectPath &handle,
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
    const QStringList addresses = options.value(QStringLiteral("addresses")).toStringList();
    const QString subject = options.value(QStringLiteral("subject")).toString();
    const QString body = options.value(QStringLiteral("body")).toString();
    const QStringList ccs = options.value(QStringLiteral("cc")).toStringList();
    const QStringList bccs = options.value(QStringLiteral("bcc")).toStringList();
    const QStringList attachments = options.value(QStringLiteral("attachments")).toStringList();

    QStringList addressList = addresses;
    addressList.prepend(address);

    QString ccString = ccs.isEmpty() ? QString() : QStringLiteral("&cc=%1").arg(ccs.join(QLatin1Char(',')));

    QString bccString = bccs.isEmpty() ? QString() : QStringLiteral("&bcc=%1").arg(bccs.join(QLatin1Char(',')));

    QString attachmentString;
    for (const QString &attachment : attachments)
        attachmentString += QStringLiteral("&attachment=%1").arg(attachment);

    const QString mailtoString = QStringLiteral("mailto:%1?subject=%2&body=%3%4%5%6")
            .arg(addressList.join(QLatin1Char(',')), subject, body, attachmentString, ccString, bccString);
    return QDesktopServices::openUrl(QUrl(mailtoString)) ? 0 : 1;
}
