import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/UIConstants.js" as UI

Sheet {
    id: logSheet

    // Лог конкретного приложения из очереди (AptLogModel). Задаётся открывающим.
    property QtObject logModel: null

    // Шапка как у CategoriesSheet: заголовок слева светлым шрифтом,
    // кнопка закрытия справа.
    title: Text {
        text: "APT log"
        font.pixelSize: 26
        font.weight: Font.Light
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter
    }

    buttons: Item {
        anchors.fill: parent
        SheetButton {
            id: closeButton
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            text: "Close"
            onClicked: close()
        }
        Component.onCompleted: closeButton.clicked.connect(rejected)
    }

    content: Rectangle {
        anchors.fill: parent
        color: UI.COLOR_BACKGROUND // штатный фон магазина

        ListView {
            id: logList
            anchors.fill: parent
            anchors.margins: 10
            clip: true
            model: logSheet.logModel
            cacheBuffer: 2000

            delegate: Text {
                width: logList.width
                font.family: "Monospace"
                font.pixelSize: 18
                textFormat: Text.PlainText
                wrapMode: Text.WrapAnywhere
                text: line
                color: level === "error"   ? "#cc0000"
                     : level === "warning" ? "#cc6600"
                     : UI.COLOR_FOREGROUND
            }

            // Показываем хвост лога — там обычно причина ошибки.
            Component.onCompleted: positionViewAtEnd()
            onCountChanged: positionViewAtEnd()
        }
    }
}
