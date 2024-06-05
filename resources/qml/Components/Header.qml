import QtQuick 1.1

Rectangle{
    z: 1
    anchors {
        top: page.top
        left: page.left
        right: page.right
    }
    width: parent.width
    height: 72
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#4DA1FF" }
        GradientStop { position: 1.0; color: "#1E90FF" }
    }
    Text {
        font.pixelSize: 32
        color: "white"
        text: title
        anchors.left: parent.left
        anchors.leftMargin: 20
        verticalAlignment: Text.AlignVCenter
        height: parent.height
    }
}
