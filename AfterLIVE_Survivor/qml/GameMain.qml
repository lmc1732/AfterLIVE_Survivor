import QtQuick
import QtQuick.Window
import QtQuick.Controls 2.15

Rectangle {
    id: gameMain
    anchors.fill: parent
    color: "transparent"  

// Background image
    Image {
        anchors.fill: parent
        source: "qrc:/assets/images/ui/game_bg.png"  
        fillMode: Image.PreserveAspectCrop  
        z: -1   // Place at the bottom layer
    }

    signal backToCharacterSelect  

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
    // ========== Top-Center Timer ==========
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

    // ========== Left-Side Stats Display ==========
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
    font.bold: false          
    style: Text.Outline
    styleColor: "#80000000"   // Semi-transparent black
}

    Text {
        text: "💚 Recovery: +" + (gameManager ? gameManager.player.hpRegen : 0) + "/s"
        color: "#51cf66"
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "🛡️ Armor: " + (gameManager ? gameManager.player.armor : 0)
        color: "#74c0fc"
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "👟 Move Speed: " + (gameManager ? gameManager.player.moveSpeed : 0)
        color: "#ffd43b"
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "⚔️ Might: " + (gameManager ? gameManager.player.power : 0)
        color: "#ff8787"
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "📐 Area: " + (gameManager ? gameManager.player.area : 0)
        color: "#a5d8ff"
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "⚡ Attack Speed: " + (gameManager ? gameManager.player.attackSpeed.toFixed(1) : 0)
        color: "#F57C00"  
        font.pixelSize: 13
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
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
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: {
            if (!gameManager) return "💢 Might Bonus: -"
            var bonus = (gameManager.player.damageMultiplier - 1) * 100
            return bonus > 0 ? "💢 Might Bonus: +" + bonus.toFixed(0) + "%" : "💢 Might Bonus: -"
        }
        color: "#ff8787"
        font.pixelSize: 12
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
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
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }

    Text {
        text: "✨ Status: " + (gameManager ? gameManager.player.activeEffects.join(", ") : "")
        color: "#ffd43b"
        font.pixelSize: 11
        wrapMode: Text.NoWrap
        width: 200
        font.bold: false
        style: Text.Outline
        styleColor: "#80000000"
    }
}
        

    

    // ========== Right-Side Health Bar and Experience Bar ==========
Column {
    id: rightStatsColumn   
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
            width: 200; height: 50; color: "#F5EDE0"; radius: 10; border.color: "#E0B87A"; border.width: 2
            Text { text: "RESUME"; color: "#5A4A3A"; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    gamePaused = false
                    if (gameManager) gameManager.setSpawnTimerEnabled(true)
                    pauseOverlay.visible = false
                }
            }
        }

        // Main Menu
        Rectangle {
            width: 200; height: 50; color: "#F5EDE0"; radius: 10; border.color: "#E0B87A"; border.width: 2
            Text { text: "MAIN MENU"; color: "#5A4A3A"; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
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
            width: 200; height: 50; color: "#F5EDE0"; radius: 10; border.color: "#D4A373"; border.width: 2
            Text { text: "QUIT"; color: "#5A4A3A"; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Qt.quit()
                }
            }
        }
    }
}

   Rectangle {
    id: player
    width: 80                  
    height: 80                 
    radius: 40                 
    color: "transparent"       

    // Original movement logic attributes
    property int speed: gameManager ? gameManager.player.moveSpeed : 280
    property bool upPressed: false
    property bool downPressed: false
    property bool leftPressed: false
    property bool rightPressed: false

    // Logical coordinates (used for movement calculation)
    property real logicX: root.width/2 - width/2
    property real logicY: root.height/2 - height/2

    // Movement function
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

    Component.onCompleted: {
        player.x = root.width/2 - width/2
        player.y = root.height/2 - height/2
    }

    // Visual sprite (as child element, automatically follows circle movement)
    Image {
        id: playerImage
     
        width: 90
        height: 90
        anchors.centerIn: parent
        source: {
            var character = gameManager ? gameManager.selectedCharacter : 0
            var basePath = "qrc:/assets/images/player/"
            if (character === 0) {
                return (player.leftPressed && !player.rightPressed) ? basePath + "CHERRY_left.png" : basePath + "CHERRY_right.png"
            } else {
                return (player.leftPressed && !player.rightPressed) ? basePath + "GANGANJI_left.png" : basePath + "GANGANJI_right.png"
            }
        }
        fillMode: Image.PreserveAspectFit
        smooth: true
    }
}

 Repeater {
    model: gameManager ? gameManager.enemies : []
    delegate: Item {
        property real displaySize: {
            switch(modelData.type) {
                case 0: return 60;   // Moss Ball
                case 1: return 70;   // Branch Gunner
                case 2: return 60;   // Berry Bomb
                case 3: return 70;   // Bark Shield
                case 4: return 100;  // Racoon Boss
                case 5: return 40;   // Small Berry
                default: return 60;
            }
        }
        x: modelData.x - displaySize/2
        y: modelData.y - displaySize/2
        width: displaySize
        height: displaySize
        Component.onCompleted: console.log("Enemy type:", modelData.type, "facingRight:", modelData.facingRight)
        Image {
            anchors.fill: parent
            source: {
                var type = modelData.type
                var facingRight = modelData.facingRight
                var basePath = "qrc:/assets/images/enemies/"
                if (type === 0) return facingRight ? basePath + "moss_ball_right.png" : basePath + "moss_ball_left.png"
                else if (type === 1) return facingRight ? basePath + "branch_gunner_right.png" : basePath + "branch_gunner_left.png"
                else if (type === 2) return facingRight ? basePath + "berry_bomb_right.png" : basePath + "berry_bomb_left.png"
                else if (type === 3) {
                    // Bark Shield: Switch image based on blocking status
                    if (modelData.isBlocking)
                        return facingRight ? basePath + "bark_shield_glow_right.png" : basePath + "bark_shield_glow_left.png"
                    else
                        return facingRight ? basePath + "bark_shield_right.png" : basePath + "bark_shield_left.png"
                }
                else if (type === 4) return facingRight ? basePath + "raccoon_boss_right.png" : basePath + "raccoon_boss_left.png"
                else if (type === 5) return facingRight ? basePath + "small_berry_right.png" : basePath + "small_berry_left.png"
                return ""
            }
            fillMode: Image.PreserveAspectFit
            smooth: true
            sourceSize.width: 128
            sourceSize.height: 128
        }
       
    }
}

   // Player Bullet
Repeater {
    model: {
        if (!gameManager) return []
        var list = []
        for (var i = 0; i < gameManager.bullets.length; i++) {
            var bullet = gameManager.bullets[i]
            if (!bullet.isRootProjectile && bullet.isPlayerBullet && !bullet.isRebounded) list.push(bullet)
        }
        return list
    }
    delegate: Image {
        width: (gameManager.selectedCharacter === 0) ? 32 : 48
        height: (gameManager.selectedCharacter === 0) ? 32 : 48
        x: modelData.x - width/2
        y: modelData.y - height/2
        source: {
            if (gameManager.selectedCharacter === 0)
                return "qrc:/assets/images/bullets/carrot.png"
            else
                return "qrc:/assets/images/bullets/bone.png"
        }
        rotation: modelData.angle
        transformOrigin: Item.Center
        smooth: true
        sourceSize.width: 128
        sourceSize.height: 128
    }
}

Repeater {
    model: {
        if (!gameManager) return []
        var list = []
        for (var i = 0; i < gameManager.bullets.length; i++) {
            var bullet = gameManager.bullets[i]
            if (!bullet.isPlayerBullet && !bullet.isRebounded && !bullet.isBossBullet) list.push(bullet)
        }
        return list
    }
    delegate: Image {
        width: 56
        height: 56
        x: modelData.x - width/2
        y: modelData.y - height/2
        source: "qrc:/assets/images/bullets/branch_bullet.png"
        rotation: Math.atan2(modelData.velY, modelData.velX) * 180 / Math.PI
        transformOrigin: Item.Center
        smooth: true
        sourceSize.width: 128
        sourceSize.height: 128
    }
}

// Boss Bullet (Acorn, tip faces movement direction)
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
    delegate: Image {
        width: 32
        height: 32
        x: modelData.x - width/2
        y: modelData.y - height/2
        source: "qrc:/assets/images/bullets/acorn_bullet.png"
        rotation: Math.atan2(modelData.velY, modelData.velX) * 180 / Math.PI
        transformOrigin: Item.Center
        smooth: true
        sourceSize.width: 128
        sourceSize.height: 128
    }
}// Explosion 
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
    Image {
        width: modelData.radius * 2
        height: modelData.radius * 2
        x: modelData.x - width/2
        y: modelData.y - height/2
        source: "qrc:/assets/images/effects/spore_fog.png"
        fillMode: Image.PreserveAspectFit
        opacity: 0.6
        NumberAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 8000
        }
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
                        width: 220
                        height: 250
                        color: "#DCD3C6"
                        radius: 15
                        border.color:  "#E6B87A"
                        border.width: 4

                       
                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            width: parent.width - 20

                            Image {
                               source: model.icon
                               width: 96
                               height: 96
                               fillMode: Image.PreserveAspectFit
                               anchors.horizontalCenter: parent.horizontalCenter
                               smooth: true
                               sourceSize.width: 256
                               sourceSize.height: 256
                            }

                            Text {
                                text: model.name
                                color: "#5A5045"
                                font.pixelSize: 20
                                font.bold: true
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            Text {
                                text: model.description
                                color:  "#8A7F72" 
                                font.pixelSize: 15
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
    visible: false
    z: 300   
    
    Image {
        anchors.fill: parent
        source: "qrc:/assets/images/ui/gameover.png"
        fillMode: Image.Stretch      
        smooth: true
        sourceSize.width: 800
        sourceSize.height: 600
    }

    // Transparent button area
    Item {
        anchors.fill: parent

        // Restart (Return to Main Menu) Button
        MouseArea {
            x: 180
            y: 520
            width: 140
            height: 30
            onClicked: {
                gameMain.backToCharacterSelect()
            }
        }

        // Quit Button
        MouseArea {
            x: 483
            y: 517
            width: 140
            height: 30
            onClicked: {
                Qt.quit()
            }
        }
    }

    onVisibleChanged: {
        if (pauseButtonItem) pauseButtonItem.visible = !visible
    }
}

    // Victory Overlay
Rectangle {
    id: victoryOverlay
    anchors.fill: parent
    visible: false
    z: 300

    onVisibleChanged: {
        pauseButtonItem.visible = !visible
    }

    Image {
        anchors.fill: parent
        source: "qrc:/assets/images/ui/victory.png"  
        fillMode: Image.PreserveAspectCrop
    }

    Item {
        anchors.fill: parent
        // Return to Main Menu Button
        MouseArea {
            x: 187
            y: 503
            width: 160
            height: 50
            onClicked: {
                gameMain.backToCharacterSelect()
            }
        }
        // Quit Button
        MouseArea {
            x: 530
            y: 505
            width: 140
            height: 50
            onClicked: {
                Qt.quit()
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