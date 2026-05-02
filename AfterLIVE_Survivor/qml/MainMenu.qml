import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
    id: mainMenu
    anchors.fill: parent
    color: "black"

    signal startGameClicked

    readonly property int designWidth: 1706
    readonly property int designHeight: 1279
    property real scaleFactor: Math.min(
        (parent ? parent.width : designWidth) / designWidth,
        (parent ? parent.height : designHeight) / designHeight
    )
    property real containerX: (parent.width - designWidth * scaleFactor) / 2
    property real containerY: (parent.height - designHeight * scaleFactor) / 2
    property real containerWidth: designWidth * scaleFactor
    property real containerHeight: designHeight * scaleFactor

    Item {
        x: containerX
        y: containerY
        width: containerWidth
        height: containerHeight

        Image {
            anchors.fill: parent
            source: "qrc:/assets/images/ui/main_menu_bg.jpg"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            x: 614 * scaleFactor
            y: 696 * scaleFactor
            width: 500 * scaleFactor
            height: 100 * scaleFactor
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                console.log("START")
                mainMenu.startGameClicked()
            }
        }

        MouseArea {
            x: 615 * scaleFactor
            y: 810 * scaleFactor
            width: 500 * scaleFactor
            height: 100 * scaleFactor
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                console.log("STORY")
                infoOverlay.visible = true
            }
        }

        MouseArea {
            x: 610 * scaleFactor
            y: 960 * scaleFactor
            width: 500 * scaleFactor
            height: 100 * scaleFactor
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                console.log("QUIT")
                Qt.quit()
            }
        }
    }

    Rectangle {
        id: infoOverlay
        anchors.fill: parent
        color: "#80000000"
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }

        Rectangle {
            width: 540
            height: 420
            color: "#FDFAF2"
            radius: 20
            border.color: "#FFD366"
            border.width: 2
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 15
                width: parent.width - 48

                Text {
                    text: "STORY"
                    color: "#5A4A3A"
                    font.pixelSize: 28
                    font.bold: true
                    font.family: "Times New Roman"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    width: parent.width
                    text:  "<i>AfterLIVE: Survivor</i><br><br>" +
          "Deep in the oak forest guarded by Mr. Raccoon, a mafia-style game spiraled completely out of control.<br>" +
          "The forest creatures could no longer tell game apart from reality, throwing the whole woods into chaos as strange distorted beings ran amok.<br><br>" +
          "MINIVE stepped into the forest to calm the turmoil, only to be pulled into this endless survival trial.<br><br>" +
          "Hold your ground amid the chaotic woods, endure wave after wave of relentless foes,<br>" +
          "and outlive all others to become the final survivor.<br><br>" +
          "After the game, you are the Survivor."
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                    font.family: "Times New Roman"
                    color: "#5A4A3A"
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    width: 100
                    height: 40
                    color: "#FFD366"
                    radius: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        text: "Close"
                        color: "#5A4A3A"
                        font.pixelSize: 16
                        font.bold: true
                        font.family: "Times New Roman"
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