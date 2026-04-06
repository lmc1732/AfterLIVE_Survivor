import QtQuick
import QtQuick.Window
import QtQuick.Controls 2.15

Rectangle {
    id: gameMain
    anchors.fill: parent
    color: "white"  

    signal backToCharacterSelect  // signal

// Reset overlay and player state when the UI is shown
    onVisibleChanged: {
        if (visible) {
            gameOverOverlay.visible = false
            victoryOverlay.visible = false
            player.x = parent.width/2 - player.width/2
            player.y = parent.height/2 - player.height/2
            player.resetKeys()
            focusScope.forceActiveFocus()
        }
    }



    // Global game loop
    Timer {
        id: gameLoop
        interval: 16
        running: true
        repeat: true
        property real lastTimestamp: 0
        onTriggered: {
            if (gamePaused) return;  
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

    Timer {
    id: refreshTimer
    interval: 100   // Refresh every 100 milliseconds
    running: true
    repeat: true
    onTriggered: {
        if (gameManager) {
            gameManager.refreshEnemies()
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
        Text {
            text: "✨ Status: " + (gameManager ? gameManager.player.activeEffects.join(", ") : "")
            color: "#ffd43b"
            font.pixelSize: 11
            wrapMode: Text.NoWrap
            width: 200
        }
}
        

    

    // ========== Health Bar and Experience Bar on the right side ==========
Column {
    id: rightStatsColumn   // Add an ID for locating the pause button
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
                return parent.width * (currentExp / neededForNext)
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


// Add an attribute to the GameMain root element
property bool gamePaused: false

// Pause Button
Item {
    id: pauseButton
    width: 40
    height: 40
    anchors.right: parent.right
    anchors.rightMargin: 10
    anchors.top: rightStatsColumn.bottom
    anchors.topMargin: 10
    z: 200

    MouseArea {
        anchors.fill: parent
        onClicked: {
            gamePaused = true
            if (gameManager) gameManager.setSpawnTimerEnabled(false)
            pauseOverlay.visible = true
        }
    }

    Text {
        text: "⏸"
        color: "white"
        font.pixelSize: 24
        anchors.centerIn: parent
    }
}

// Pause Popup
Rectangle {
    id: pauseOverlay
    anchors.fill: parent
    color: "#80000000"
    visible: false
    z: 1000

    MouseArea { anchors.fill: parent; onClicked: { } }

    Column {
        anchors.centerIn: parent
        spacing: 20

        // Resume
        Rectangle {
            width: 200; height: 50; color: "#4caf50"; radius: 10
            Text { text: "Resume"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    gamePaused = false
                    if (gameManager) gameManager.setSpawnTimerEnabled(true)
                    pauseOverlay.visible = false
                }
            }
        }

        // Return Main Menu
        Rectangle {
            width: 200; height: 50; color: "#2196f3"; radius: 10
            Text { text: "Main Menu"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    gamePaused = false
                    if (gameManager) gameManager.setSpawnTimerEnabled(true)
                    pauseOverlay.visible = false
                    gameMain.backToCharacterSelect()
                }
            }
        }

        // Quit
        Rectangle {
            width: 200; height: 50; color: "#f44336"; radius: 10
            Text { text: "Quit"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Qt.quit()
                }
            }
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

    // Enemies (color varies by type)
    Repeater {
        model: gameManager ? gameManager.enemies : []
        Rectangle {
            width: modelData.size
            height: modelData.size
            color: {
                // Bark Shield turns gold when blocking
                if (modelData.type === 3 && modelData.isBlocking === true) return "#FFD700";
                switch(modelData.type) {
                    case 0: return "#8B4513";  // MossBall Brown
                    case 1: return "#2E8B57";  // BranchGunner Green
                    case 2: return "#8B008B";  // BerryBomb Violet
                    case 3: return "#A0522D";  // BarkShield Tan
                    case 4: return "#FF4500";  // RaccoonBoss Orange
                    default: return "purple";
                }
            }
            radius: 15
            x: modelData.x - width/2
            y: modelData.y - height/2     
        }
        
    }

   // Normal Bullet (yellow circle)
Repeater {
    model: {
        if (!gameManager) return []
        var list = []
        for (var i = 0; i < gameManager.bullets.length; i++) {
            var bullet = gameManager.bullets[i]
            if (!bullet.isRootProjectile) list.push(bullet)
        }
        return list
    }
    delegate: Rectangle {
        width: 8
        height: 8
        color: "yellow"
        radius: 4
        x: modelData.x - width/2
        y: modelData.y - height/2
    }
}

// Stun Bullet
Repeater {
    model: {
        if (!gameManager) return []
        var list = []
        for (var i = 0; i < gameManager.bullets.length; i++) {
            var bullet = gameManager.bullets[i]
            if (bullet.isRootProjectile) list.push(bullet)
        }
        return list
    }
    delegate: Item {
        id: bulletRoot
        x: modelData.x - width/2
        y: modelData.y - height/2
        width: 130
        height: 16   // Max Height (Tail)

        property real angle: Math.atan2(modelData.velY, modelData.velX) * 180 / Math.PI
        rotation: angle

        // Tail
        Rectangle {
            width: 50
            height: 16
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#0A2463" }
                GradientStop { position: 1.0; color: "#1E88E5" }
            }
            opacity: 0.85
            radius: 4
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        // Head (narrow, tapering to a point)
        Rectangle {
            width: 80
            height: 6
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#1E88E5" }
                GradientStop { position: 0.6; color: "#4FC3F7" }
                GradientStop { position: 1.0; color: "#E0F7FA" }
            }
            opacity: 0.9
            radius: 3
            anchors.left: parent.left
            anchors.leftMargin: 45   // Overlap with tail by 5 pixels for smooth transition
            anchors.verticalCenter: parent.verticalCenter
        }

        // Center bright band (runs from head to tail)
        Rectangle {
            width: parent.width - 10
            height: 2
            color: "#CCF5FF"
            opacity: 0.8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        // Tip Glow
        Rectangle {
            width: 12
            height: 12
            color: "white"
            radius: 6
            anchors.right: parent.right
            anchors.rightMargin: -4
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0.9
            layer.enabled: true
            layer.effect: Qt.linalg.blur
        }

        // Tail fade (transparent gradient)
        Rectangle {
            width: 40
            height: 20
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#FFFFFF" }
                GradientStop { position: 1.0; color: "#00000000" }
            }
            anchors.left: parent.left
            anchors.leftMargin: -5
            opacity: 0.5
        }

        // Overall breathing animation
        SequentialAnimation on scale {
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 1.03; duration: 300; easing.type: Easing.InOutQuad }
            NumberAnimation { from: 1.03; to: 1.0; duration: 300; easing.type: Easing.InOutQuad }
        }
    }
}

// Boss Bullet (red circle with glow)
Repeater {
    model: {
        if (!gameManager) return []
        var list = []
        for (var i = 0; i < gameManager.bullets.length; i++) {
            var bullet = gameManager.bullets[i]
            if (bullet.isBossBullet) list.push(bullet)
        }
        return list
    }
    delegate: Item {
        x: modelData.x - width/2
        y: modelData.y - height/2
        width: 16
        height: 16

        Rectangle {
            width: parent.width
            height: parent.height
            color: "#FF3300"
            radius: 8
            border.color: "#FFAA00"
            border.width: 1
        }
        // Glow
        Rectangle {
            width: parent.width + 8
            height: parent.height + 8
            color: "transparent"
            border.color: "#FF6600"
            border.width: 2
            radius: 10
            opacity: 0.6
            anchors.centerIn: parent
        }
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

    Repeater {
    model: gameManager ? gameManager.sporeMistEffects : []
    Rectangle {
        width: modelData.radius * 2
        height: modelData.radius * 2
        color: "#aaffaa"
        opacity: 0.4
        radius: modelData.radius
        x: modelData.x - width/2
        y: modelData.y - height/2
    }
}

    // Level Up Choice Popup
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
        visible: false
        z: 100

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
                    text: "Restart"
                    color: "white"
                    font.pixelSize: 20
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameMain.backToCharacterSelect()
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
                    text: "Quit"
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

    // Victory Overlay
    Rectangle {
        id: victoryOverlay
        anchors.fill: parent
        color: "black"
        opacity: 0.7
        visible: false
        z: 100

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "VICTORY!"
                color: "gold"
                font.pixelSize: 48
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "You defeated the boss!"
                color: "white"
                font.pixelSize: 24
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Survival Time: " + (gameManager ? Math.floor(gameManager.gameTime) : 0) + " seconds"
                color: "white"
                font.pixelSize: 18
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: 200
                height: 50
                color: "#4caf50"
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: "Restart"
                    color: "white"
                    font.pixelSize: 20
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameMain.backToCharacterSelect()
                    }
                }
            }
        }
    }

    Connections {
        target: gameManager
        function onGameWon() {
            gameOverOverlay.visible = false
            victoryOverlay.visible = true
        }
        function onPlayerDied() {
            victoryOverlay.visible = false
            gameOverOverlay.visible = true
        }
    }

    // Keyboard Focus Manager
    FocusScope {
        id: focusScope
        anchors.fill: parent
        focus: true

        Keys.onPressed: function(event) {
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
        Keys.onReleased: function(event) {
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