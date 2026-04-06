import QtQuick

Rectangle {
    id: mainMenu
    anchors.fill: parent
    color: "#2c3e50"

    signal startGameClicked

    Column {
        anchors.centerIn: parent
        spacing: 30
        width: 200  // Fixed width to ensure child alignment

        Text {
            text: "After live: Survivor"
            color: "gold"
            font.pixelSize: 48
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        // Start Game Button
        Rectangle {
            width: parent.width
            height: 50
            color: "#4caf50"
            radius: 5

            Text {
                text: "Start Game"
                color: "white"
                font.pixelSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                onClicked: mainMenu.startGameClicked()
            }
        }

        // Game Info Button
        Rectangle {
            width: parent.width
            height: 50
            color: "#2196f3"
            radius: 5

            Text {
                text: "Game Desciption"
                color: "white"
                font.pixelSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                onClicked: infoOverlay.visible = true
            }
        }

        // Quit Button
        Rectangle {
            width: parent.width
            height: 50
            color: "#f44336"
            radius: 5

            Text {
                text: "Quit"
                color: "white"
                font.pixelSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }
    }

    // Game Info Popup
    Rectangle {
        id: infoOverlay
        anchors.fill: parent
        color: "#80000000"
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: { }
        }

        Rectangle {
            width: 400
            height: 200
            color: "#3E2723"
            radius: 15
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 20

                Text {
                    text: "Game Setting"
                    color: "white"
                    font.pixelSize: 24
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Rectangle {
                    width: 100
                    height: 40
                    color: "#795548"
                    radius: 5
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        text: "Close"
                        color: "white"
                        font.pixelSize: 16
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: infoOverlay.visible = false
                    }
                }
            }
        }
    }
}