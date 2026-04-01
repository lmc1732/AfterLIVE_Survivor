import QtQuick
import QtQuick.Window

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: "After live: Survivor"

    Timer {
        id: gameLoop
        interval: 16
        running: true
        repeat: true
        property real lastTimestamp: 0
        onTriggered: {
            var now = Date.now()
            var deltaTime = (lastTimestamp === 0) ? interval / 1000 : (now - lastTimestamp) / 1000
            lastTimestamp = now
            deltaTime = Math.min(deltaTime, 0.033)

            if (gameManager && !gameManager.isGameOver && !gameManager.isGamePaused) {
                player.updateMovement(deltaTime)
            }

            if (typeof gameManager !== "undefined") {
                gameManager.update(deltaTime, player.x, player.y)
            }
        }
    }

    // ========== Timer at the top center of the screen ==========
    Text {
        id: timerText
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        font.pixelSize: 24
        font.bold: true
        style: Text.Outline
        styleColor: "black"
        text: {
            if (!gameManager) return "0:00"
            var totalSeconds = Math.floor(gameManager.gameTime)
            var minutes = Math.floor(totalSeconds / 60)
            var seconds = totalSeconds % 60
            return minutes + ":" + (seconds < 10 ? "0" + seconds : seconds)
        }
    }

    // ========== Displays player attributes on the left side ==========
    Column {
        id: statsColumn
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        spacing: 5

        Text {
            text: "❤️ Max Health: " + (gameManager ? gameManager.player.maxHp : 0)
            color: "#ff6b6b"
            font.pixelSize: 13
            font.bold: true
        }

        Text {
            text: "💚 Recovery: +" + (gameManager ? gameManager.player.hpRegen : 0) + "/s"
            color: "#51cf66"
            font.pixelSize: 13
        }

        Text {
            text: "🛡️ Armor: " + (gameManager ? gameManager.player.armor : 0)
            color: "#74c0fc"
            font.pixelSize: 13
        }

        Text {
            text: "👟 Move Speed: " + (gameManager ? gameManager.player.moveSpeed : 0)
            color: "#ffd43b"
            font.pixelSize: 13
        }

        Text {
            text: "⚔️ Might: " + (gameManager ? gameManager.player.power : 0)
            color: "#ff8787"
            font.pixelSize: 13
        }

        Text {
            text: "📐 Area: " + (gameManager ? gameManager.player.area : 0)
            color: "#a5d8ff"
            font.pixelSize: 13
        }

        Text {
            text: "⚡ Attack Speed: " + (gameManager ? gameManager.player.attackSpeed.toFixed(1) : 0)
            color: "#94d82d"
            font.pixelSize: 13
        }

        Rectangle {
            width: 140
            height: 1
            color: "gray"
            opacity: 0.5
        }

        Text {
            text: {
                if (!gameManager) return "✨ Growth: -"
                var bonus = (gameManager.player.expMultiplier - 1) * 100
                return bonus > 0 ? "✨ Growth: +" + bonus.toFixed(0) + "%" : "✨ Growth: -"
            }
            color: "#ffd43b"
            font.pixelSize: 12
        }

        Text {
            text: {
                if (!gameManager) return "💢 Might Bonus: -"
                var bonus = (gameManager.player.damageMultiplier - 1) * 100
                return bonus > 0 ? "💢 Might Bonus: +" + bonus.toFixed(0) + "%" : "💢 Might Bonus: -"
            }
            color: "#ff8787"
            font.pixelSize: 12
        }

        Text {
            text: {
                var effects = []
                if (gameManager && gameManager.player.hasSlowEffect) effects.push("🏹 Slow")
                if (gameManager && gameManager.player.hasHealOnKill) effects.push("💖 Heal on Kill")
                if (gameManager && gameManager.player.hasBurnEffect) effects.push("🔥 Burn")
                return effects.length > 0 ? effects.join("  ") : ""
            }
            color: "#c5f6fa"
            font.pixelSize: 11
        }
    }

    // ========== Health Bar and Experience Bar on the right side ==========
    Column {
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        spacing: 5

        // Health Bar
        Rectangle {
            width: 200
            height: 20
            color: "gray"
            radius: 10

            Rectangle {
                width: parent.width * (gameManager.player.hp / gameManager.player.maxHp)
                height: parent.height
                color: "red"
                radius: 10
                Behavior on width { NumberAnimation { duration: 100 } }
            }
            
            Text {
                text: "❤️ " + (gameManager ? gameManager.player.hp : 0) + "/" + (gameManager ? gameManager.player.maxHp : 0)
                color: "white"
                font.pixelSize: 11
                anchors.centerIn: parent
            }
        }

        // Experience Bar
        Rectangle {
            width: 200
            height: 20
            color: "darkgray"
            radius: 10

            Rectangle {
                width: {
                    if (!gameManager) return 0
                    var currentExp = gameManager.player.exp
                    var neededForNext = gameManager.player.expToNextLevel
                    if (neededForNext <= 0) return 0
                    var ratio = currentExp / neededForNext
                    if (ratio > 1) ratio = 1
                    return parent.width * ratio
                }
                height: parent.height
                color: "blue"
                radius: 10
                Behavior on width { NumberAnimation { duration: 100 } }
            }
            
            Text {
                text: {
                    if (!gameManager) return "Lv.0 0/0"
                    var level = gameManager.player.level
                    var currentExp = gameManager.player.exp
                    var neededForNext = gameManager.player.expToNextLevel
                    return "Lv." + level + " " + currentExp + "/" + neededForNext
                }
                color: "white"
                font.pixelSize: 11
                anchors.centerIn: parent
            }
        }
    }

    // Player
    Rectangle {
        id: player
        width: 40
        height: 40
        color: "red"
        radius: 20
        x: root.width/2 - width/2
        y: root.height/2 - height/2

        property int speed: gameManager ? gameManager.player.moveSpeed : 280
        property bool upPressed: false
        property bool downPressed: false
        property bool leftPressed: false
        property bool rightPressed: false

        function updateMovement(deltaTime) {
            if (gameManager && (gameManager.isGameOver || gameManager.isGamePaused)) return

            var dx = 0, dy = 0
            if (upPressed) dy -= 1
            if (downPressed) dy += 1
            if (leftPressed) dx -= 1
            if (rightPressed) dx += 1
            if (dx !== 0 || dy !== 0) {
                var len = Math.hypot(dx, dy)
                dx /= len
                dy /= len
                player.x += dx * speed * deltaTime
                player.y += dy * speed * deltaTime
                player.x = Math.max(0, Math.min(root.width - player.width, player.x))
                player.y = Math.max(0, Math.min(root.height - player.height, player.y))
            }
        }

        function clampPosition() {
            player.x = Math.max(0, Math.min(root.width - player.width, player.x))
            player.y = Math.max(0, Math.min(root.height - player.height, player.y))
        }

        function resetKeys() {
            upPressed = false
            downPressed = false
            leftPressed = false
            rightPressed = false
        }
    }

    // Enemy
    Repeater {
        model: gameManager ? gameManager.enemies : []
        Rectangle {
            width: 30
            height: 30
            color: "purple"
            radius: 15
            x: modelData.x - width/2
            y: modelData.y - height/2
        }
    }

    // Bullet
    Repeater {
        model: gameManager ? gameManager.bullets : []
        Rectangle {
            width: 8
            height: 8
            color: "yellow"
            radius: 4
            x: modelData.x - width/2
            y: modelData.y - height/2
        }
    }

         // Explosion
    Repeater {
        model: gameManager ? gameManager.explosions : []
        Repeater {
            model: modelData.particles
            Rectangle {
                width: modelData.size
                height: modelData.size
                color: Qt.rgba(modelData.colorR / 255.0, modelData.colorG / 255.0, modelData.colorB / 255.0, 1.0)
                radius: width / 2
                x: modelData.x - width/2
                y: modelData.y - height/2
                opacity: Math.min(1.0, modelData.life / 0.3)
                Behavior on opacity { NumberAnimation { duration: 50 } }
            }
        }
    }

    // Exp Ball
    Repeater {
        model: gameManager ? gameManager.expBalls : []
        Rectangle {
            width: 16
            height: 16
            color: "gold"
            radius: 8
            x: modelData.x - width/2
            y: modelData.y - height/2

            SequentialAnimation on scale {
                loops: Animation.Infinite
                NumberAnimation { from: 0.8; to: 1.2; duration: 500 }
                NumberAnimation { from: 1.2; to: 0.8; duration: 500 }
            }
        }
    }

    // Level up choice window
    Rectangle {
        id: upgradeDialog
        anchors.fill: parent
        color: "#80000000"
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: { }
        }

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "Choose an Upgrade"
                color: "gold"
                font.pixelSize: 32
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Row {
                spacing: 30
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: upgradeModel
                    Rectangle {
                        width: 200
                        height: 250
                        color: "#2c3e50"
                        radius: 15
                        border.color: "gold"
                        border.width: 2

                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            width: parent.width - 20

                            Rectangle {
                                width: 80
                                height: 80
                                color: "gray"
                                radius: 10
                                anchors.horizontalCenter: parent.horizontalCenter

                                Text {
                                    text: "Image"
                                    color: "white"
                                    anchors.centerIn: parent
                                    font.pixelSize: 12
                                }
                            }

                            Text {
                                text: model.name
                                color: "white"
                                font.pixelSize: 18
                                font.bold: true
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            Text {
                                text: model.description
                                color: "#cccccc"
                                font.pixelSize: 14
                                anchors.horizontalCenter: parent.horizontalCenter
                                wrapMode: Text.WordWrap
                                horizontalAlignment: Text.AlignHCenter
                                width: parent.width
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                gameManager.selectUpgrade(index)
                                upgradeDialog.visible = false
                                player.resetKeys()
                                focusScope.forceActiveFocus()
                                player.clampPosition()
                            }
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: upgradeModel
    }

    Connections {
        target: gameManager
        function onShowUpgradeOptions(names, descriptions, icons) {
            upgradeModel.clear()
            for (var i = 0; i < names.length; i++) {
                upgradeModel.append({"name": names[i], "description": descriptions[i], "icon": icons[i]})
            }
            upgradeDialog.visible = true
        }
        function onGamePausedChanged() {
            if (!gameManager.isGamePaused && upgradeDialog.visible) {
                upgradeDialog.visible = false
                player.resetKeys()
                focusScope.forceActiveFocus()
                player.clampPosition()
            }
        }
    }

    // Game Over Overlay
    Rectangle {
        id: gameOverOverlay
        anchors.fill: parent
        color: "black"
        opacity: 0.7
        visible: gameManager ? gameManager.isGameOver : false

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "GAME OVER"
                color: "red"
                font.pixelSize: 48
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Level: Lv." + (gameManager ? gameManager.player.level : 1)
                color: "white"
                font.pixelSize: 24
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: {
                    if (!gameManager) return "Survival Time: 0:00"
                    var totalSeconds = Math.floor(gameManager.gameTime)
                    var minutes = Math.floor(totalSeconds / 60)
                    var seconds = totalSeconds % 60
                    return "Survival Time: " + minutes + ":" + (seconds < 10 ? "0" + seconds : seconds)
                }
                color: "white"
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: 200
                height: 50
                color: "#4caf50"
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: "RESTART"
                    color: "white"
                    font.pixelSize: 20
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameManager.restartGame()
                        player.x = root.width/2 - player.width/2
                        player.y = root.height/2 - player.height/2
                        player.resetKeys()
                        focusScope.forceActiveFocus()
                    }
                }
            }

            Rectangle {
                width: 200
                height: 50
                color: "#f44336"
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: "QUIT"
                    color: "white"
                    font.pixelSize: 20
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Qt.quit()
                    }
                }
            }
        }
    }

    FocusScope {
        id: focusScope
        anchors.fill: parent
        focus: true

        Keys.onPressed: {
            if (gameManager && (gameManager.isGameOver || gameManager.isGamePaused)) {
                event.accepted = true
                return
            }
            switch (event.key) {
            case Qt.Key_W: case Qt.Key_Up: player.upPressed = true; break
            case Qt.Key_S: case Qt.Key_Down: player.downPressed = true; break
            case Qt.Key_A: case Qt.Key_Left: player.leftPressed = true; break
            case Qt.Key_D: case Qt.Key_Right: player.rightPressed = true; break
            }
            event.accepted = true
        }
        Keys.onReleased: {
            if (gameManager && (gameManager.isGameOver || gameManager.isGamePaused)) {
                event.accepted = true
                return
            }
            switch (event.key) {
            case Qt.Key_W: case Qt.Key_Up: player.upPressed = false; break
            case Qt.Key_S: case Qt.Key_Down: player.downPressed = false; break
            case Qt.Key_A: case Qt.Key_Left: player.leftPressed = false; break
            case Qt.Key_D: case Qt.Key_Right: player.rightPressed = false; break
            }
            event.accepted = true
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!gameManager || (!gameManager.isGameOver && !gameManager.isGamePaused)) {
                focusScope.forceActiveFocus()
            }
        }
    }

    Component.onCompleted: {
        focusScope.forceActiveFocus()
        console.log("Game started")
    }
}