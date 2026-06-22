import QtQuick 1.1

// Image, который при ошибке загрузки/декодирования (битый или неподдерживаемый
// формат — напр. .jpeg на симуляторе) ИЛИ при пустом source автоматически
// показывает fallbackSource. Использовать как обычный Image: задать source и
// fallbackSource (+ при желании fillMode/smooth). Заменяет ручной приём с флагом
// failed на AppsPage/AppPage/HomePage/DeveloperPage.
//
// Сделан обёрткой над внутренним Image, чтобы переключение на fallback было
// ДЕКЛАРАТИВНЫМ: при смене source (в т.ч. при переиспользовании делегата ListView)
// флаг ошибки сбрасывается и снова пробуется основной источник.
Item {
    id: root

    property url source
    property url fallbackSource
    property int fillMode: Image.PreserveAspectFit
    property bool smooth: true
    property alias status: img.status

    property bool __failed: false
    onSourceChanged: __failed = false // новый источник — снова пробуем основной

    Image {
        id: img
        anchors.fill: parent
        fillMode: root.fillMode
        smooth: root.smooth
        source: (root.__failed || root.source == "") ? root.fallbackSource : root.source
        onStatusChanged: if (status === Image.Error && root.fallbackSource != "") root.__failed = true
    }
}
