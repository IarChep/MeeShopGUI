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

    function restore_style() {updateProgressBar.style = standartStyle; installProgressBar.style = standartStyle}

    ProgressBarStyle {
        id: standartStyle
    }

    ProgressBarStyle {
        id: sucsessfulStyle
        unknownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString + "-bar-unknown-texture"
        knownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString +"-bar-known-texture"
    }

    ProgressBarStyle {
        id: errorStyle
        unknownTexture: "image://theme/color14-meegotouch-progressindicator"+ __invertedString + "-bar-unknown-texture"
        knownTexture: "image://theme/color14-meegotouch-progressindicator"+ __invertedString +"-bar-known-texture"
    }

    Item {
        anchors.top: parent.top
        id: updateItem
        width: parent.width
        height: childrenRect.height
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressBar {
                id: updateProgressBar
                width: 300
                maximumValue: 8
                minimumValue: 0
                value: im.repeatCount
            }

            Text {
                id: updateInfo
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
    Item {
        anchors.top: updateItem.bottom
        id: installItem
        width: parent.width
        height: childrenRect.height
        visible: false
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressBar {
                id: installProgressBar
                anchors.horizontalCenter: parent.horizontalCenter
                maximumValue: 100
                minimumValue: 0
                width: 300
            }
            Text {
                id: installInfo
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
        to: installItem.visible ? installRect.childrenRect.height : installRect.childrenRect.height / 2
        duration: 260

    }
    PropertyAnimation {
        id: closeAnimation
        target: installRect
        properties: "height"
        to: 0.0
        duration: 300
    }

    SequentialAnimation {
        id: installationError
        PropertyAnimation { target: installProgressBar; property: "x"; to: target.x + 10; duration: 50 }
        PropertyAnimation { target: installProgressBar; property: "x"; to: target.x - 20; duration: 100 }
        PropertyAnimation { target: installProgressBar; property: "x"; to: target.x + 10; duration: 50 }
    }
    SequentialAnimation {
        id: updatingError
        PropertyAnimation { target: updateProgressBar; property: "x"; to: target.x + 10; duration: 50 }
        PropertyAnimation { target: updateProgressBar; property: "x"; to: target.x - 20; duration: 100 }
        PropertyAnimation { target: updateProgressBar; property: "x"; to: target.x + 10; duration: 50 }
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
            updateProgressBar.style = errorStyle
            updatingError.start()
            updateInfo.text = "Update finished"
            openAnimation.start()
        }
    }
}
