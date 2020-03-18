// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "screenshotimageprovider.h"

ScreenshotImageProvider::ScreenshotImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ScreenshotImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id)
    Q_UNUSED(requestedSize)

    *size = image.size();
    return image;
}
