import QtQuick 1.1
import "../../js/UIConstants.js" as Ui

// Передний план стандартного заголовка: только текст/индикатор/клик (без фона —
// фон задаётся отдельным слоем в HeaderBar). Заполняет свой контейнер; может быть
// кликабельным — клик активен лишь пока эта страница текущая.
Item {
    id: root

    anchors.fill: parent

    property string title: ""
    property bool clickable: false
    signal clicked()

    Text {
        font.pixelSize: 32
        color: "white"
        text: root.title
        anchors.left: parent.left
        anchors.leftMargin: 20
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        opacity: mouseArea.pressed ? 0.5 : 1
    }
    Image {
        visible: root.clickable
        anchors {
            right: parent.right
            rightMargin: 15
            verticalCenter: parent.verticalCenter
        }
        source: "image://theme/meegotouch-combobox-indicator-inverted"
        opacity: mouseArea.pressed ? 0.5 : 1
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.clickable
        onClicked: root.clicked()
    }
}
