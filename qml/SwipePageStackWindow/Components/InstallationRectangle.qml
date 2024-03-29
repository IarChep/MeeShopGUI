// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: installRect
    anchors.top: parent.bottom
    color: "transparent"
    width: parent.width
    clip: true

    function start_installation() {
        updateItem.visible = true
        openAnimation.start()
        processManager.update_repositories()
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
                maximumValue: 100
                minimumValue: 0
                width: 300
                value: processManager.updatePercentage
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: processManager.updateOutput === "" ? "Updating packages..." : processManager.updateOutput
            }
        }
    }
    Item {
        anchors.top: updateInfo.bottom
        id: installItem
        width: parent.width
        height: childrenRect.height
        visible: false
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressBar {
                id: installationProgressBar
                maximumValue: 200
                minimumValue: 0
                width: 300
                value: processManager.installationPercentage
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: processManager.installationOutput === "" ? "Installing application..." : processManager.installationOutput
            }
        }
    }


    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
    }
    Connections {
        target: processManager

        onUpdate_finished: {
            if (sucsess) {
                updateProgressBar.style = sucsessfulStyle;
                installItem.visible = true
                openAnimation.start()
                processManager.install_package(argList[3])
            } else {
                updateProgressBar.style = errorStyle
                updateErrorAnimtion.start()
                installButt.text = "Error!"
                installButt.enabled = true
            }
        }
        onInstallation_finished: {
            if (sucsess) {
                installationProgressBar.value = 200
                installationProgressBar.style = sucsessfulStyle
                installButt.text = "Installed!"
            } else {
                installationProgressBar.style = errorStyle
                installationErrorAnimtion.start()
                installButt.text = "Error!"
                installButt.enabled = true
            }
        }
    }

    //progressbar styles
    ProgressBarStyle {
        id: standartStyle
    }

    ProgressBarStyle {
        id: errorStyle
        unknownTexture: "image://theme/color14-meegotouch-progressindicator"+ __invertedString + "-bar-unknown-texture"
        knownTexture: "image://theme/color14-meegotouch-progressindicator"+ __invertedString +"-bar-known-texture"
    }
    ProgressBarStyle {
        id: sucsessfulStyle
        unknownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString + "-bar-unknown-texture"
        knownTexture: "image://theme/color2-meegotouch-progressindicator"+ __invertedString +"-bar-known-texture"
    }

    //animations
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
    SequentialAnimation {
        id: updateErrorAnimtion
        NumberAnimation {target: updateProgressBar; to: 10; duration: 50}
        NumberAnimation {target: updateProgressBar; to: -10; duration: 100}
        NumberAnimation {target: updateProgressBar; to: 0; duration: 50}
    }

    SequentialAnimation {
        id: installationErrorAnimtion
        NumberAnimation {target: installationProgressBar; to: 10; duration: 50}
        NumberAnimation {target: installationProgressBar; to: -10; duration: 100}
        NumberAnimation {target: installationProgressBar; to: 0; duration: 50}
    }
}
