import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: characterSelect
    anchors.fill: parent
    color: "transparent"   // Background fully covered by image

    // Grid background
    Image {
        anchors.fill: parent
        source: "qrc:/assets/images/ui/character_select_bg.png"
        fillMode: Image.Stretch
        opacity: 0.85
        z: -1
    }

    signal characterSelected(int type)

    Column {
        anchors.centerIn: parent
        spacing: 30          

        // Title
        Item {
            width: 500
            height: 80
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                source: "qrc:/assets/images/decorations/cherry.png"
                width: 48
                height: 48
                sourceSize.width: 144
                sourceSize.height: 144
                smooth: true
                anchors.right: titleText.left
                anchors.rightMargin: 20
                anchors.verticalCenter: titleText.verticalCenter
            }

            Text {
                id: titleText
                text: "Select Your Character"
                color: "#5A4A3A"
                font.pixelSize: 34
                font.bold: true
                font.family: "Times New Roman"
                anchors.centerIn: parent
            }

            Image {
                source: "qrc:/assets/images/decorations/shine_muscat.png"
                width: 48
                height: 48
                sourceSize.width: 144
                sourceSize.height: 144
                smooth: true
                anchors.left: titleText.right
                anchors.leftMargin: 20
                anchors.verticalCenter: titleText.verticalCenter
            }
        }

        Row {
            spacing: 60      
            anchors.horizontalCenter: parent.horizontalCenter

            // ========== Player1：CHERRY ==========
            Item {
                width: 240
                height: 320

                // Card Shadow
                Rectangle {
                    anchors.fill: parent
                    color: "#A0A0A0"
                    opacity: 0.08
                    radius: 20
                    y: 4
                }

                // Main card (semi-transparent white, blending into the macaron background)
                Rectangle {
                    width: 200
                    height: 280
                    color: "#FDFAF2"
                    opacity: 0.92
                    radius: 20
                    border.color: "#E0C8D0"   // Light Pink
                    border.width: 2

                    Column {
                        anchors.fill: parent
                        spacing: 10
                        topPadding: 20

                        Image {
                            source: "qrc:/assets/images/characters/CHERRY.png"
                            width: 160
                            height: 160
                            fillMode: Image.PreserveAspectFit
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "CHERRY"
                            color: "#B97A3B"
                            font.pixelSize: 22
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "✦ High HP ✦ Cone Attack ✦"
                            color: "#7A6A5A"
                            font.pixelSize: 11
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Rectangle {
                            width: 30
                            height: 2
                            color: "#E0C8D0"
                            radius: 1
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            gameManager.selectCharacter(0)
                            characterSelected(0)
                        }
                    }
                }
            }

            // ========== Player2：GANGANJI ==========
            Item {
                width: 240
                height: 320

                Rectangle {
                    anchors.fill: parent
                    color: "#A0A0A0"
                    opacity: 0.08
                    radius: 20
                    y: 4
                }

                Rectangle {
                    width: 200
                    height: 280
                    color: "#FDFAF2"
                    opacity: 0.92
                    radius: 20
                    border.color: "#C0D4E0"   // Light Blue
                    border.width: 2

                    Column {
                        anchors.fill: parent
                        spacing: 10
                        topPadding: 20

                        Image {
                            source: "qrc:/assets/images/characters/GANGANJI.png"
                            width: 160
                            height: 160
                            fillMode: Image.PreserveAspectFit
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "GANGANJI"
                            color: "#B97A3B"
                            font.pixelSize: 22
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "✦ Fast Attack ✦ Homing Explosion ✦"
                            color: "#7A6A5A"
                            font.pixelSize: 11
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Rectangle {
                            width: 30
                            height: 2
                            color: "#C0D4E0"
                            radius: 1
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            gameManager.selectCharacter(1)
                            characterSelected(1)
                        }
                    }
                }
            }
        }
    }
}