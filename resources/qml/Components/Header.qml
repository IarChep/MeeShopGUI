import QtQuick 1.1

Rectangle{
    id: root
    z: 1
    anchors {
        top: page.top
        left: page.left
        right: page.right
    }
    property bool clickable: false
    signal clicked()
    width: parent.width
    height: 72
    color: "black"
    Image {
        source: "../../images/headerImage.png"
        anchors.fill: parent
        opacity: mouseArea.pressed ? 0.5 : 1
        Text {
            font.pixelSize: 32
            color: "white"
            text: title
            anchors.left: parent.left
            anchors.leftMargin: 20
            verticalAlignment: Text.AlignVCenter
            height: parent.height
        }
        Image {
            visible: clickable
            anchors {
                right: parent.right
                rightMargin: 15
                verticalCenter: parent.verticalCenter
            }
            source: "image://theme/meegotouch-combobox-indicator-inverted"
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            enabled: clickable
            onClicked: root.clicked()
        }
    }
}
