// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

FluidControls.ApplicationWindow {
    id: dialog

    property int what: Screenshooter.Screens
    property bool overlayCursor: true
    property url savedUrl

    signal accepted()
    signal rejected()

    title: qsTr("Screenshot")

    width: 500
    height: 350
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height

    initialPage: firstPageComponent

    Material.primary: Material.BlueGrey
    Material.accent: Material.BlueGrey

    FluidControls.SnackBar {
        id: snackBar
    }

    Component {
        id: firstPageComponent

        FluidControls.Page {
            title: qsTr("Screenshot")
            enabled: Screenshooter.enabled

            Column {
                anchors.fill: parent

                /*
                FluidControls.ListItem {
                    text: qsTr("Grab")
                    rightItem: ComboBox {
                        anchors.verticalCenter: parent.verticalCenter
                        textRole: "text"
                        model: ListModel {
                            ListElement { text: qsTr("All screens"); value: "screens" }
                            //ListElement { text: qsTr("Area"); value: "select-area" }
                        }
                        implicitWidth: 200
                        onCurrentIndexChanged: what = currentIndex
                    }
                }
                */

                FluidControls.ListItem {
                    text: qsTr("Delay")
                    rightItem: SpinBox {
                        anchors.verticalCenter: parent.verticalCenter
                        from: 1
                        to: 60
                        onValueModified: shootTimer.interval = value * 1000

                        Component.onCompleted: {
                            value = shootTimer.interval / 1000;
                        }
                    }
                }

                FluidControls.ListItem {
                    text: qsTr("Include pointer")
                    rightItem: Switch {
                        anchors.verticalCenter: parent.verticalCenter
                        onCheckedChanged: {
                            overlayCursor = checked;
                        }

                        Component.onCompleted: {
                            checked = overlayCursor;
                        }
                    }
                }
            }

            footer: DialogButtonBox {
                Button {
                    text: qsTr("Capture")
                    flat: true
                    onClicked: {
                        dialog.hide();
                        shootTimer.start();
                    }
                }
            }
        }
    }

    Component {
        id: previewPageComponent

        FluidControls.Page {
            title: qsTr("Preview")
            enabled: Screenshooter.enabled

            footer: DialogButtonBox {
                Button {
                    text: qsTr("Cancel")
                    flat: true

                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole

                    onClicked: {
                        dialog.rejected();
                    }
                }

                Button {
                    text: qsTr("Copy to clipboard")
                    flat: true

                    onClicked: {
                        Screenshooter.copyToClipboard();
                    }
                }

                Button {
                    text: qsTr("Save As")
                    flat: true

                    onClicked: {
                        fileDialog.open();
                    }
                }

                Button {
                    text: qsTr("Save")
                    flat: true

                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                    onClicked: {
                        var url = Qt.resolvedUrl("file://" + Screenshooter.generateFileName());
                        if (Screenshooter.saveScreenshot(url)) {
                            savedUrl = url;
                            dialog.accepted();
                        } else {
                            snackBar.open(qsTr("Failed to save screenshot"));
                        }
                    }
                }
            }

            Image {
                property real ratio: sourceSize.width / sourceSize.height

                anchors.centerIn: parent
                width: parent.width * 0.8
                height: width / ratio

                source: "image://screenshooter/last"
                fillMode: Image.Stretch
                cache: false

                layer.enabled: true
                layer.effect: FluidEffects.Elevation {
                    elevation: 2
                }
            }
        }
    }

    Timer {
        id: shootTimer
        interval: 2000
        onTriggered: {
            Screenshooter.takeScreenshot(what, overlayCursor);
        }
    }

    Timer {
        id: showTimer
        interval: 1000
        onTriggered: {
            dialog.pageStack.push(previewPageComponent);
            dialog.show();
        }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Save Screenshot As")
        fileMode: FileDialog.SaveFile
        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
        nameFilters: [ qsTr("Image files (*.jpg *.jpeg *.png)"), qsTr("All files (*)") ]
        onAccepted: {
            if (Screenshooter.saveScreenshot(fileDialog.file)) {
                savedUrl = fileDialog.file;
                dialog.accepted();
            } else {
                snackBar.open(qsTr("Failed to save screenshot"));
            }
        }
    }

    Connections {
        target: Screenshooter
        onScreenshotDone: showTimer.start()
    }
}
