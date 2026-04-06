import QtQuick
import QtQuick.Controls

Rectangle {
    id: characterSelect
    anchors.fill: parent
    color: "#2c3e50"

    signal characterSelected(int type)

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "Select Your Character"
            color: "white"
            font.pixelSize: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter

            // PlayerA
            Rectangle {
                width: 200
                height: 250
                color: "#34495e"
                radius: 15
                border.color: "gold"
                border.width: 2

                Column {
                    anchors.centerIn: parent
                    spacing: 15

                    Rectangle {
                        width: 80
                        height: 80
                        color: "#ff6b6b"
                        radius: 40
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text { text: "🍕"; font.pixelSize: 50; anchors.centerIn: parent }
                    }

                    Text { text: "PlayerA"; color: "white"; font.pixelSize: 20; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
                    Text { text: "High HP\nCone Attack"; color: "#cccccc"; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter; anchors.horizontalCenter: parent.horizontalCenter }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameManager.selectCharacter(0)
                        characterSelected(0)
                    }
                }
            }

            // PlayerB
            Rectangle {
                width: 200
                height: 250
                color: "#34495e"
                radius: 15
                border.color: "gold"
                border.width: 2

                Column {
                    anchors.centerIn: parent
                    spacing: 15

                    Rectangle {
                        width: 80
                        height: 80
                        color: "#51cf66"
                        radius: 40
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text { text: "🦴"; font.pixelSize: 50; anchors.centerIn: parent }
                    }

                    Text { text: "PlayerB"; color: "white"; font.pixelSize: 20; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
                    Text { text: "High Attack Speed\nHoming Bullet+ Explosion"; color: "#cccccc"; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter; anchors.horizontalCenter: parent.horizontalCenter }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameManager.selectCharacter(1)
                        characterSelected(1)
                    }
                }
            }
        }
    }
}