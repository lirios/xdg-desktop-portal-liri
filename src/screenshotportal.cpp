// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QEventLoop>
#include <QGuiApplication>
#include <QTimer>

#include "desktopportal.h"
#include "logging_p.h"
#include "quickdialog.h"
#include "screenshotclient.h"
#include "screenshotportal.h"
#include "waylandintegration.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Screenshot.xml

// Keep in sync with qflatpakcolordialog from Qt flatpak platform theme
Q_DECLARE_METATYPE(ScreenshotPortal::ColorRGB)

QDBusArgument &operator<<(QDBusArgument &arg, const ScreenshotPortal::ColorRGB &color)
{
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, ScreenshotPortal::ColorRGB &color)
{
    double red, green, blue;
    arg.beginStructure();
    arg >> red >> green >> blue;
    color.red = red;
    color.green = green;
    color.blue = blue;
    arg.endStructure();

    return arg;
}

//////

ScreenshotPortal::ScreenshotPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<ColorRGB>();
}

quint32 ScreenshotPortal::Screenshot(const QDBusObjectPath &handle,
                                     const QString &app_id,
                                     const QString &parent_window,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    qCDebug(lcScreenshot) << "Screenshot called with parameters:";
    qCDebug(lcScreenshot) << "    handle: " << handle.path();
    qCDebug(lcScreenshot) << "    app_id: " << app_id;
    qCDebug(lcScreenshot) << "    parent_window: " << parent_window;
    qCDebug(lcScreenshot) << "    options: " << options;

    auto *client = DesktopPortal::instance()->screenshotClient();
    if (!client->isEnabled()) {
        qCWarning(lcScreenshot, "Screen copy extension is not ready");
        return 1;
    }

    const bool modal = options.value(QStringLiteral("modal"), true).toBool();
    const bool interactive = options.value(QStringLiteral("interactive"), false).toBool();

    if (interactive) {
        auto *dialog = new QuickDialog(QUrl(QLatin1String("qrc:/qml/ScreenshotDialog.qml")));
        dialog->setModal(modal);
        if (dialog->exec()) {
            auto fileName = dialog->rootObject()->property("savedUrl").toUrl().toLocalFile();
            results.insert(QStringLiteral("uri"), fileName);
            dialog->deleteLater();
            return 0;
        }

        dialog->deleteLater();
        return 1;
    } else {
        client->takeScreenshot(ScreenshotClient::AllScreens, true);

        auto fileName = client->generateFileName();
        if (client->saveScreenshot(fileName)) {
            results.insert(QStringLiteral("uri"), fileName);
            return 0;
        }
    }

    return 1;
}

quint32 ScreenshotPortal::PickColor(const QDBusObjectPath &handle,
                                    const QString &app_id,
                                    const QString &parent_window,
                                    const QVariantMap &options,
                                    QVariantMap &results)
{
    qCDebug(lcScreenshot) << "PickColor called with parameters:";
    qCDebug(lcScreenshot) << "    handle: " << handle.path();
    qCDebug(lcScreenshot) << "    app_id: " << app_id;
    qCDebug(lcScreenshot) << "    parent_window: " << parent_window;
    qCDebug(lcScreenshot) << "    options: " << options;

    auto *colorPicker = WaylandIntegration::instance()->colorPicker();
    if (!colorPicker->isActive()) {
        qCWarning(lcScreenshot, "Color picker extension is not ready");
        return 1;
    }

    quint32 returnedSerial = 0;
    QColor selectedColor;

    // Pick color interactively and wait for an answer within 30 seconds
    quint32 ourSerial = colorPicker->pickInteractively(QGuiApplication::primaryScreen());
    QEventLoop loop;
    connect(colorPicker, &LiriColorPickerManager::colorPicked, this, [&](quint32 serial, const QColor &color) {
        returnedSerial = serial;
        selectedColor = color;
        loop.quit();
    });
    QTimer::singleShot(30000, &loop, &QEventLoop::quit);
    loop.exec();
    if (returnedSerial == ourSerial && selectedColor.isValid()) {
        ColorRGB color;
        color.red = selectedColor.redF();
        color.green = selectedColor.greenF();
        color.blue = selectedColor.blueF();

        results.insert(QStringLiteral("color"), QVariant::fromValue<ScreenshotPortal::ColorRGB>(color));
        return 0;
    }

    return 1;
}
