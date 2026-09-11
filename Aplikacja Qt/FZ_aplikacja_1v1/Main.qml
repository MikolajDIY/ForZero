import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 650
    height: 480
    minimumWidth: 550
    minimumHeight: 420
    visible: true
    title: "forZero Telemetry Cluster"
    color: "#f4f5f7"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // --- 1. GÓRNY PASEK STATUSU ORAZ KONFIGURACJI PORTU ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            // Wybór Portu COM
            Text {
                text: "Port COM:"
                font.pixelSize: 14
                font.bold: true
                color: "#2c3e50"
            }

            ComboBox {
                id: portComboBox
                implicitWidth: 140
                model: audiCluster.availablePorts()


                onActivated: function(index) {
                    var selectedPort = portComboBox.textAt(index)
                    console.log("Wybrano port:", selectedPort)
                    audiCluster.setPort(selectedPort)
                }

                Component.onCompleted: {
                    if (portComboBox.count > 0) {
                        audiCluster.setPort(portComboBox.textAt(0))
                    }
                }
            }

            Button {
                text: "Odśwież"
                palette.buttonText: "#2c3e50"
                onClicked: {
                    portComboBox.model = audiCluster.availablePorts()
                }
            }

            // Elastyczny odstęp spychający status USB na prawą stronę
            Item { Layout.fillWidth: true }

            // Widget statusu USB
            Rectangle {
                id: usbStatusRect
                implicitWidth: 120
                implicitHeight: 36
                color: "white"
                border.color: audiCluster.usbStats.is_connected ? "#2ecc71" : "#e74c3c"
                border.width: 1.5
                radius: 6

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 6

                    Text {
                        text: "USB:"
                        font.bold: true
                        font.pixelSize: 13
                        color: "#34495e"
                    }

                    Text {
                        text: audiCluster.usbStats.is_connected ? "OK" : "BRAK"
                        color: audiCluster.usbStats.is_connected ? "#27ae60" : "#c0392b"
                        font.bold: true
                        font.pixelSize: 13
                    }
                }

                ToolTip.visible: mouseArea.containsMouse
                ToolTip.delay: 200
                ToolTip.text: {
                    var textContent = "Wysłano paczek: " + audiCluster.usbStats.packets_sent + "\n";
                    textContent += "Błędy timeout: " + audiCluster.usbStats.timeoutFrames + "\n";
                    var list = audiCluster.usbStats.toList();
                    for (var i = 0; i < list.length; i++) {
                        textContent += list[i].name + ": " + (list[i].value / 1000.0).toFixed(1) + " ms\n";
                    }
                    return textContent.trim();
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        // --- 2. SEKCJA TELEMETRII (GŁÓWNE WSKAŹNIKI) ---
        GroupBox {
            title: "Telemetria na Żywo"
            Layout.fillWidth: true
            font.bold: true

            RowLayout {
                anchors.fill: parent
                spacing: 20

                // Prędkość
                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 80
                    color: "#ffffff"
                    radius: 8
                    border.color: "#e0e0e0"

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2

                        Text {
                            text: "PRĘDKOŚĆ"
                            font.pixelSize: 11
                            font.bold: true
                            color: "#7f8c8d"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Text {
                            text: (audiCluster.cluster.speed_m ?? 0) + " km/h"
                            font.pixelSize: 26
                            font.bold: true
                            color: "#2c3e50"
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }

                // Obroty
                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 80
                    color: "#ffffff"
                    radius: 8
                    border.color: "#e0e0e0"

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2

                        Text {
                            text: "OBROTY SILNIKA"
                            font.pixelSize: 11
                            font.bold: true
                            color: "#7f8c8d"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Text {
                            text: (audiCluster.cluster.rpm_m ?? 0) + " RPM"
                            font.pixelSize: 26
                            font.bold: true
                            color: "#2c3e50"
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }

        // --- 3. SEKCJA USTAWIEŃ SYMULACJI ---
        GroupBox {
            title: "Ustawienia Symulacji"
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.bold: true

            GridLayout {
                anchors.fill: parent
                columns: 2
                columnSpacing: 20
                rowSpacing: 10

                Switch {
                    text: "Skalowanie RPM"
                    checked: audiCluster.settings.SimClusterScalingRPM
                    onToggled: {
                        var s = audiCluster.settings;
                        s.SimClusterScalingRPM = checked;
                        audiCluster.settings = s;
                    }
                }

                Switch {
                    text: "Symulacja ABS"
                    checked: audiCluster.settings.SimClusterABS
                    onToggled: {
                        var s = audiCluster.settings;
                        s.SimClusterABS = checked;
                        audiCluster.settings = s;
                    }
                }

                Switch {
                    text: "Symulacja Temp. Silnika"
                    checked: audiCluster.settings.SimClusterEngineTemp
                    onToggled: {
                        var s = audiCluster.settings;
                        s.SimClusterEngineTemp = checked;
                        audiCluster.settings = s;
                    }
                }

                Switch {
                    text: "Symulacja TSC / Kontrola Trakcji"
                    checked: audiCluster.settings.SimClusterTSC
                    onToggled: {
                        var s = audiCluster.settings;
                        s.SimClusterTSC = checked;
                        audiCluster.settings = s;
                    }
                }

                Switch {
                    text: "Symulacja AirBag"
                    checked: audiCluster.settings.SimClusterAirBag
                    onToggled: {
                        var s = audiCluster.settings;
                        s.SimClusterAirBag = checked;
                        audiCluster.settings = s;
                    }
                }
            }
        }
    }
}
