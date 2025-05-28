import QtQuick 1.1

Rectangle {
    width: 480
    height: 90
    color: "#e0e1e2"

    Image {
        id: rotatingImage
        anchors.centerIn: parent
        source: "image://theme/icon-s-refresh"
        rotation: 0
        transformOrigin: Item.Center

        // Анимация вращения против часовой стрелки
        NumberAnimation on rotation {
            id: rotationAnim
            from: 0
            to: -360
            duration: 1000
            loops: Animation.Infinite
            running: false
            easing.type: Easing.Linear
        }
    }

    function startRotation() {
        if (!rotationAnim.running)
            rotationAnim.running = true
    }

    function stopRotation() {
        rotationAnim.running = false
        rotatingImage.rotation = 0
    }
}
