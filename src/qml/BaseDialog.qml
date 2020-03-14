// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls

Page {
    id: page

    property alias title: dialogLabel.text

    signal accepted()
    signal rejected()

    header: FluidControls.DialogLabel {
        id: dialogLabel

        padding: 24
    }

    footer: DialogButtonBox {}
}
