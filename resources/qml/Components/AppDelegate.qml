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
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            source: "http://wunderwungiel.pl/MeeGo/openrepos/icons/" + appIcon
        }
        Column {
            Text {
                text: appdelegate.higlightLetters === true && page.sort === "title" ? "<b>"+appName.charAt(0)+"</b>"+appName.slice(1) : appName
                color: "black"
                font.pixelSize: 25
            }
            Text {
                text: appdelegate.higlightLetters === true && page.sort === "publisher" ? "Size: " + appSize + " KB, Dev: " + "<b>"+appDev.charAt(0)+"</b>"+appDev.slice(1) :  "Size: " + appSize + " KB, Dev: " +appDev
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
