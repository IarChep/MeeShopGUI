import QtQuick 1.0
import com.nokia.meego 1.1

Rectangle {
    color: "#e0e1e2"
    anchors.fill: parent
    BusyIndicator {
        style: BusyIndicatorStyle {size: "big"}
        running: true
        anchors.centerIn: parent
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
