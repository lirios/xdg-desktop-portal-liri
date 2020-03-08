// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls
import QtAccountsService 1.0
import io.liri.Portal 1.0

QuickDialog {
    id: dialog

    property string appName: ""
    property string reason: ""

    title: qsTr("Share user information")

    width: page.implicitWidth
    height: page.implicitHeight

    visible: true

    UserAccount {
        id: userAccount
    }

    Page {
        id: page

        anchors.left: parent.left
        anchors.top: parent.top

        header: FluidControls.DialogLabel {
            text: qsTr("Share user information")
            padding: 24
        }

        Column {
            FluidControls.BodyLabel {
                text: qsTr("Do you really want to share this personal information with the \"%1\" application?").arg(appName || qsTr("n.a."))
            }

            FluidControls.BodyLabel {
                text: qsTr("Reason: %1").arg(reason)
                visible: reason !== ""
            }

            Row {
                Image {
                    source: userAccount.iconFileName ? "file://" + userAccount.iconFileName : ""
                }

                Column {
                    FluidControls.TitleLabel {
                        text: userAccount.displayName
                    }

                    FluidControls.CaptionLabel {
                        text: userAccount.userName
                    }
                }
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
                text: qsTr("Share")

                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                onClicked: {
                    dialog.accepted();
                    dialog.close();
                }
            }
        }
    }
}
