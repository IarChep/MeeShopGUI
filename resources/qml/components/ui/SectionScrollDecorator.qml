import QtQuick 1.1

// Скролл-декоратор «системного» вида (нативные ассеты темы meegotouch-position-
// indicator — те же, что рисует штатный com.nokia.meego ScrollDecorator) с
// модификацией: по правому краю можно вести пальцем для быстрой навигации по
// алфавиту, и при этом показывается «пузырь» с текущей буквой.
//
// Почему не штатный SectionScroller: он зовёт model.get(i), которого у C++-моделей
// в QtQuick1 нет. Секции берём из C++-модели: listView.model.sections() /
// firstIndexOfSection(). Размещать СИБЛИНГОМ ListView, anchors.fill — на область
// списка (отдельный ScrollDecorator на странице больше не нужен).
Item {
    id: root

    property Item listView          // ListView (он же Flickable) с C++-моделью секций
    property int stripWidth: 40     // ширина зоны захвата у правого края

    property bool __dragging: false
    property string __letter: ""
    property real __touchY: 0
    property bool __scrollable: listView ? (listView.contentHeight > listView.height + 1) : false

    // Геометрия индикатора из visibleArea (повтор логики системного сайзера).
    property int  __margin: 8
    property real __trackH: height - 2 * __margin
    property int  __minH: 24
    property real __indH: listView ? Math.max(__minH, listView.visibleArea.heightRatio * __trackH) : 0
    property real __indY: listView ? (__margin + Math.max(0, Math.min(listView.visibleArea.yPosition * __trackH,
                                                                      __trackH - __indH))) : 0

    function __update(my) {
        if (!listView || !listView.model)
            return;
        var secs = listView.model.sections();
        if (!secs || secs.length === 0) {
            root.__dragging = false;
            return;
        }
        root.__touchY = my;
        var h = dragArea.height;
        var f = (h > 0) ? my / h : 0;
        if (f < 0) f = 0;
        if (f > 0.99999) f = 0.99999;
        var idx = Math.floor(f * secs.length);
        if (idx < 0) idx = 0;
        if (idx >= secs.length) idx = secs.length - 1;

        root.__dragging = true;
        var letter = secs[idx];
        if (letter !== root.__letter) {
            root.__letter = letter;
            var rowIdx = listView.model.firstIndexOfSection(letter);
            if (rowIdx >= 0)
                listView.positionViewAtIndex(rowIdx, ListView.Beginning);
        }
    }

    // Виден при прокрутке списка или при перетаскивании по полосе (как системный).
    property bool __shown: root.__scrollable && ((listView && listView.moving) || root.__dragging)

    // Фон дорожки (нативный ассет темы), у правого края.
    Image {
        id: track
        source: "image://theme/meegotouch-positionindicator-background"
        anchors { right: parent.right; top: parent.top; bottom: parent.bottom
                  topMargin: root.__margin; bottomMargin: root.__margin }
        visible: root.__scrollable
        opacity: indicator.opacity
    }

    // Индикатор позиции (нативный ассет). Двигается по visibleArea, в т.ч. при
    // быстрой прокрутке (positionViewAtIndex меняет contentY -> visibleArea).
    BorderImage {
        id: indicator
        source: "image://theme/meegotouch-positionindicator-indicator"
        border { left: 2; top: 4; right: 2; bottom: 4 }
        anchors.right: parent.right
        y: root.__indY
        height: root.__indH
        visible: root.__scrollable
        opacity: root.__shown ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    // Полоса захвата у правого края: ведём пальцем -> быстрая прокрутка по буквам.
    // Обычная прокрутка списка остаётся на всей остальной площади.
    MouseArea {
        id: dragArea
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
        width: root.stripWidth
        enabled: root.__scrollable
        onPressed: root.__update(mouseY)
        onPositionChanged: root.__update(mouseY)
        onReleased: root.__dragging = false
        onCanceled: root.__dragging = false
    }

    // Пузырь с текущей буквой — рядом с индикатором, следует за пальцем; виден
    // только во время быстрой прокрутки по декоратору.
    Rectangle {
        id: bubble
        width: 96
        height: 96
        radius: 14
        color: "#dd1d1d1d"
        visible: root.__dragging && root.__letter.length > 0
        anchors { right: dragArea.left; rightMargin: 10 }
        y: {
            var yy = root.__touchY - height / 2;
            if (yy < 0) yy = 0;
            if (yy > root.height - height) yy = root.height - height;
            return yy;
        }
        Text {
            anchors.centerIn: parent
            text: root.__letter
            color: "white"
            font.pixelSize: 60
            font.bold: true
        }
    }
}
