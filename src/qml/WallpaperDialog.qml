// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls
import io.liri.Portal 1.0

QuickDialog {
    id: dialog

    property string setOn: "both"
    property alias uri: image.source

    title: qsTr("Set Background")

    width: page.implicitWidth
    height: page.implicitHeight

    visible: true

    Page {
        id: page

        anchors.left: parent.left
        anchors.top: parent.top

        header: FluidControls.DialogLabel {
            text: qsTr("Set Background")
            padding: 24
        }

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
                    dialog.close();
                }
            }

            Button {
                text: qsTr("Set")

                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                onClicked: {
                    dialog.accepted();
                    dialog.close();
                }
            }
        }
    }
}
