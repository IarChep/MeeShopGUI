import QtQuick 1.0
import com.nokia.meego 1.1

Rectangle {
    id: root
    color: "#e0e1e2"
    anchors.fill: parent

    // Необязательная подпись под индикатором (что грузится). Пусто -> скрыта.
    property alias statusText: label.text

    BusyIndicator {
        id: busy
        style: BusyIndicatorStyle {size: "big"}
        running: true
        anchors.centerIn: parent
    }

    Text {
        id: label
        text: ""
        visible: text.length > 0
        color: "#5a5a5a"
        font.pixelSize: 20
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors {
            top: busy.bottom
            topMargin: 18
            horizontalCenter: busy.horizontalCenter
        }
    }

    NumberAnimation on opacity {
        id: fadeOutAnimation
        from: 1.0
        to: 0.0
        duration: 300
        running: false
    }
    function hide() {
        fadeOutAnimation.start(); // Запускаем анимацию уменьшения прозрачности
    }

    // Функция для показа прямоугольника без анимации
    function show() {
        visible = true; // Показываем прямоугольник
        opacity = 1.0; // Восстанавливаем непрозрачность без анимации
    }
}
