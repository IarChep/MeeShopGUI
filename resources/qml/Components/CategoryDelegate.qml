import QtQuick 1.1
import com.nokia.meego 1.1
import IarChep.MeeShop 1.0

Rectangle {
    id: catDelegate

    property bool hasKids: categoryKids > 1
    signal clicked()
    signal selectButtonClicked()

    width: parent.width
    height: 90
    color: (mouseArea.pressed === true || catSheet.selectedIndex === categoryId)? "#bbbcbe" : "#e0e1e2"

    Image {
        visible: index === 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader-background"
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
        visible: !catDelegate.hasKids
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        source: "image://theme/icon-m-toolbar-next"
    }

    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader-background"
    }


    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: catDelegate.clicked()
    }
    Row {
        spacing: 15
        visible: catDelegate.hasKids
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        Image {
            anchors.verticalCenter: parent.verticalCenter
            source: "image://theme/meegotouch-combobox-indicator"
        }
        NokiaShape {
            color: mArea.pressed === true ? Qt.darker("gray", 1.2) : "gray"
            width: 50
            height: width
            Image {
                anchors.centerIn: parent
                source: "image://theme/icon-m-toolbar-next"
            }
            MouseArea {
                id: mArea
                anchors.fill: parent
                onClicked: catDelegate.selectButtonClicked()
            }
        }
    }
}
