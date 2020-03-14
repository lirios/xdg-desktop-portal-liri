// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls

BaseDialog {
    id: dialog

    property alias subtitle: subtitleLabel.text
    property alias body: bodyLabel.text
    property alias grantLabel: grantButton.text
    property alias denyLabel: denyButton.text

    Column {
        FluidControls.BodyLabel {
            id: subtitleLabel

            visible: text !== ""
        }

        FluidControls.BodyLabel {
            id: bodyLabel

            visible: text !== ""
        }
    }

    footer: DialogButtonBox {
        Button {
            id: denyButton

            text: qsTr("Deny")

            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole

            onClicked: {
                dialog.rejected();
            }
        }

        Button {
            id: acceptButton

            text: qsTr("Accept")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

            onClicked: {
                dialog.accepted();
            }
        }
    }
}
