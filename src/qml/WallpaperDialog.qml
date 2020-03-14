// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls

BaseDialog {
    id: dialog

    property string setOn: "both"
    property alias uri: image.source

    title: qsTr("Set Background")

    Column {
        Label {
            text: {
                if (setOn === "background")
                    qsTr("Please confirm if you want to change the wallpaper of your desktop.")
                else if (setOn === "lockscreen")
                    qsTr("Please confirm if you want to change the background of the lock screen.")
                else
                    qsTr("Please confirm if you want to change the wallpaper of your desktop and the background of the lock screen.")
            }
        }

        Image {
            id: image

            width: 500
            height: 300
        }
    }

    footer: DialogButtonBox {
        Button {
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole

            onClicked: {
                dialog.rejected();
            }
        }

        Button {
            text: qsTr("Set")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

            onClicked: {
                dialog.accepted();
            }
        }
    }
}
