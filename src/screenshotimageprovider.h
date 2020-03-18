// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENSHOTIMAGEPROVIDER_H
#define SCREENSHOTIMAGEPROVIDER_H

#include <QQuickImageProvider>

class ScreenshotImageProvider : public QQuickImageProvider
{
public:
    ScreenshotImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    QImage image;
};

#endif // SCREENSHOTIMAGEPROVIDER_H
