/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.10
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls
import io.liri.Portal 1.0

QuickDialog {
    id: appChooserDialog

    property string selectedAppId: ""

    width: 400
    height: 400

    visible: true

    Page {
        anchors.fill: parent

        ScrollView {
            anchors.fill: parent
            clip: true

            ListView {
                model: appsModel
                header: FluidControls.Subheader {
                    text: qsTr("Open with")
                }
                delegate: FluidControls.ListItem {
                    icon.name: model.iconName
                    text: model.name
                    highlighted: model.preferred
                    onClicked: appChooserDialog.selectedAppId = model.id
                }
            }
        }

        footer: DialogButtonBox {
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
            onAccepted: {
                appChooserDialog.accepted(appChooserDialog.selectedAppId);
                appChooserDialog.close();
            }
            onRejected: {
                appChooserDialog.rejected();
                appChooserDialog.close();
            }
        }
    }
}
