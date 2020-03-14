// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls
import io.liri.Portal 1.0

FluidCore.Object {
    Component {
        id: accessDialogComponent

        AccessDialog {}
    }

    function newAccessDialog() {
        return accessDialogComponent.createObject();
    }
}
