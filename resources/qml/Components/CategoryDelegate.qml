import QtQuick 1.1

Rectangle {
    id: catDelegate

    signal clicked()

    width: parent.width
    height: 90
    color: mouseArea.pressed === true ? "#bbbcbe" : "#e0e1e2"

    Image {
        visible: index === 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
    }

    Text {
        anchors {
            left: parent.left
            leftMargin: 15
            verticalCenter: parent.verticalCenter
        }
        font.pixelSize: 26
        font.bold: true
        text: categoryName + " (" + categoryAmount + " apps)"
    }

    Image {
        visible: categoryKids > 1
        anchors {
            right: parent.right
            rightMargin: 15
            verticalCenter: parent.verticalCenter
        }
        source: "image://theme/meegotouch-combobox-indicator"
    }

    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: catDelegate.clicked()
    }
}
