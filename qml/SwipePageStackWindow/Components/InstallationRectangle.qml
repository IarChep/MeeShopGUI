// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: installRect
    anchors.top: parent.bottom
    color: "transparent"
    width: parent.width
    clip: true

    function installation_started() {
        openAnimation.start()
    }

    ProgressBarStyle {
        id: sucsessfulStyle
        unknownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString + "-bar-unknown-texture"
        knownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString +"-bar-known-texture"
    }

    Item {
        anchors.top: parent.top
        id: updateInfo
        width: parent.width
        height: childrenRect.height
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressBar {
                id: updateProgressBar
                anchors.horizontalCenter: parent.horizontalCenter
                maximumValue: 100
                minimumValue: 0
                width: 300
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "ponn"
            }
        }
    }
    Item {
        anchors.top: updateInfo.bottom
        id: installInfo
        width: parent.width
        height: childrenRect.height
        visible: false
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressBar {
                anchors.horizontalCenter: parent.horizontalCenter
                maximumValue: 100
                minimumValue: 0
                width: 300
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "ponn"
            }
        }
    }


    PropertyAnimation {
        id: openAnimation
        target: installRect
        properties: "height"
        easing.type: Easing.OutQuad
        to: installInfo.visible ? installRect.childrenRect.height : installRect.childrenRect.height / 2
        duration: 260

    }
    PropertyAnimation {
        id: closeAnimation
        target: installRect
        properties: "height"
        to: 0.0
        duration: 300
    }


    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
    }
    Connections {
        target: im

        onUpdatingRepositoresFinished: {
            updateProgressBar.style = sucsessfulStyle
        }
    }
}
