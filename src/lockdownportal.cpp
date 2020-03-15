// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lockdownportal.h"

LockdownPortal::LockdownPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // We don't have lockdown settings for now
    setProperty("disable-printing", false);
    setProperty("disable-save-to-disk", false);
    setProperty("disable-application-handlers", false);
    setProperty("disable-location", false);
    setProperty("disable-camera", false);
    setProperty("disable-microphone", false);
    setProperty("disable-sound-output", false);
}
