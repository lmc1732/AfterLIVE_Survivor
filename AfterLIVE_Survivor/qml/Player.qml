import QtQuick

Rectangle {
    id: root
    width: 40
    height: 40
    color: "red"
    radius: 20

    property int speed: 300
    property bool up: false
    property bool down: false
    property bool left: false
    property bool right: false

    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            var dx = 0, dy = 0
            if (root.up) dy -= 1
            if (root.down) dy += 1
            if (root.left) dx -= 1
            if (root.right) dx += 1
            if (dx !== 0 || dy !== 0) {
                var len = Math.hypot(dx, dy)
                dx /= len
                dy /= len
                root.x += dx * root.speed * (interval / 1000)
                root.y += dy * root.speed * (interval / 1000)
                root.x = Math.max(0, Math.min(root.parent.width - root.width, root.x))
                root.y = Math.max(0, Math.min(root.parent.height - root.height, root.y))
            }
        }
    }
}