import QtQuick
import QtQuick.Window

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: "After live: Survivor"

    property string currentScreen: "menu"   // "menu", "character", "game"
    property int selectedCharacter: 0

    function showMenu() { currentScreen = "menu" }
    function showCharacterSelect() { currentScreen = "character" }
    function startGame() {
        currentScreen = "game"
    }
    function endGame() {
        currentScreen = "menu"   // Return to main menu
        gameManager.restartGame() // Reset game data
    }

    MainMenu {
        visible: currentScreen === "menu"
        anchors.fill: parent
        onStartGameClicked: showCharacterSelect()
    }

    CharacterSelect {
        visible: currentScreen === "character"
        anchors.fill: parent
        onCharacterSelected: {
            selectedCharacter = type
            startGame()
        }
    }

    GameMain {
        id: gameMain
        visible: currentScreen === "game"
        anchors.fill: parent
        onBackToCharacterSelect: endGame()
    }

    Component.onCompleted: {
        currentScreen = "menu"
    }
}