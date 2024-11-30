import QtQuick 1.1
import com.nokia.extras 1.1


Rectangle {
    id: appdelegate

    property bool higlightLetters: false
    signal clicked()

    width: parent.width
    height: 90
    color: mouseArea.pressed === true ? "#bbbcbe" : "#e0e1e2"

    Row {
        x: UiConstants.DefaultMargin + 15
        anchors.verticalCenter: parent.verticalCenter
        spacing: 15
        Image {
            width: 65
            height: width
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            source: appIcon === undefined  ? "image://theme/icon-m-content-ovi-store-inverse" : appIcon
        }
        Column {
            Text {
                text: appName
                color: "black"
                font.pixelSize: 25
            }
            Text {
                text:  "By: " +appDev
                color: "black"
                font.pixelSize: 20
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: appdelegate.clicked()
    }
}
