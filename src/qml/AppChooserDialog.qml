// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls

BaseDialog {
    id: dialog

    property string selectedAppId: ""
    property string fileName: ""
    property string uri: ""
    property string contentType: ""
    property string fileType: ""

    title: qsTr("Open with")

    width: 400
    height: 400

    Page {
        anchors.fill: parent

        Column {
            anchors.fill: parent

            FluidControls.BodyLabel {
                text: {
                    if (fileName || uri)
                        return qsTr("Choose an application to open \"%1\".").arg(fileName || uri);
                    else if (fileType)
                        return qsTr("Choose an application to open a file of type \"%1\".").arg(fileType);
                    else
                        return "";
                }
                visible: text !== ""
            }

            ScrollView {
                clip: true

                ListView {
                    model: appsModel
                    delegate: FluidControls.ListItem {
                        icon.name: model.iconName
                        text: model.name
                        highlighted: model.preferred
                        onClicked: dialog.selectedAppId = model.id
                    }
                }
            }
        }

        footer: DialogButtonBox {
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
            onAccepted: {
                dialog.accepted();
            }
            onRejected: {
                dialog.rejected();
            }
        }
    }
}
