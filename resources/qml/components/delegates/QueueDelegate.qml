import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "../ui"

// Элемент очереди установки: иконка (с ExtendedIndicator), название, разработчик,
// статус-текст, и колонка маленьких кнопок (удалить / лог) в правом верхнем углу.
//
// Перетаскивание (смена порядка): высоты делегатов НЕ меняются, поэтому раскладка
// ListView не искажается и contentY остаётся стабильным (важно для точного
// хит-теста). Место под перетаскиваемый элемент освобождается сдвигом СОДЕРЖИМОГО
// делегатов (translate), а сам перетаскиваемый делегат прячет своё содержимое.
Item {
    id: del

    property int itemHeight: 100

    // Человекочитаемое имя операции (для текста статуса у ожидающих задач).
    function __opName(op) {
        return op === InstallationQueue.Install    ? "Install"
             : op === InstallationQueue.Update     ? "Update"
             : op === InstallationQueue.Remove     ? "Remove"
             : op === InstallationQueue.RepoUpdate ? "Repo update"
             : "Install";
    }

    // Состояние перетаскивания страница прокидывает в ListView.
    property int dragIndex:  ListView.view ? ListView.view.dragIndex  : -1
    property int dropTarget: ListView.view ? ListView.view.dropTarget : -1
    property bool reordering: dragIndex >= 0
    property bool isDragged: index === dragIndex

    // Сдвиг содержимого, освобождающий место в позиции dropTarget.
    // Тащим вниз  -> элементы (dragIndex; dropTarget] едут вверх на один слот.
    // Тащим вверх -> элементы [dropTarget; dragIndex) едут вниз на один слот.
    property int shift: (!reordering || isDragged) ? 0
        : (dropTarget > dragIndex) ? ((index > dragIndex && index <= dropTarget) ? -itemHeight : 0)
        : (dropTarget < dragIndex) ? ((index >= dropTarget && index < dragIndex) ?  itemHeight : 0)
        : 0

    // «Свободное место» появляется ПЛАВНО: при изменении сдвига во время
    // перетаскивания анимируем content.y; при входе/выходе из режима (reordering
    // выкл.) — мгновенно. Используем явную NumberAnimation, а НЕ Behavior, потому
    // что её можно ОСТАНОВИТЬ: иначе незавершённая на момент отпускания анимация
    // «доезжала» бы до старой цели и давала рывок (тот самый баг с дёрганьем).
    onShiftChanged: {
        if (del.reordering) {
            gapShiftAnim.to = del.shift;
            gapShiftAnim.restart();
        } else {
            gapShiftAnim.stop();
            content.y = del.shift;
        }
    }
    NumberAnimation {
        id: gapShiftAnim
        target: content
        property: "y"
        duration: 180
        easing.type: Easing.InOutQuad
    }

    width: ListView.view ? ListView.view.width : 480
    height: itemHeight
    clip: false // содержимое съезжает в соседние слоты — обрезку делает сам ListView

    // Анимация удаления: схлопывание высоты + затухание, затем фактическое удаление
    // (соседние делегаты при этом съезжаются, закрывая место). clip:true на время
    // схлопывания обрезает содержимое фиксированной высоты по уменьшающемуся делегату.
    states: State {
        name: "removing"
        PropertyChanges { target: del; height: 0; opacity: 0; clip: true }
    }
    transitions: Transition {
        to: "removing"
        SequentialAnimation {
            // Держим делегат живым на время анимации (не уничтожится при отскролле
            // за пределы cacheBuffer), иначе ScriptAction не выполнится.
            PropertyAction { target: del; property: "ListView.delayRemove"; value: true }
            NumberAnimation { target: del; properties: "height,opacity"; duration: 220; easing.type: Easing.InOutQuad }
            ScriptAction {
                script: {
                    var id = model.appId;
                    queue.cancel(id);
                    // cancel() откажет, если строка успела стать Running (processNext)
                    // за время анимации — тогда она осталась в модели; возвращаем её
                    // (через Timer, чтобы не менять state реентрантно внутри транзишена).
                    if (queue.taskStatus(id) >= 0)
                        restoreTimer.start();
                }
            }
            PropertyAction { target: del; property: "ListView.delayRemove"; value: false }
        }
    }

    // Возврат строки, если удаление было отклонено (задача успела стать Running).
    Timer {
        id: restoreTimer
        interval: 0
        onTriggered: del.state = ""
    }

    // Видимая строка делегата. Её и сдвигаем, освобождая место.
    Rectangle {
        id: content
        width: parent.width
        height: del.itemHeight
        // content.y задаётся анимацией gapShiftAnim (плавно во время перетаскивания)
        // или мгновенно при входе/выходе из режима — НЕ биндим, чтобы анимацию можно
        // было остановить. Перетаскиваемый прячется мгновенно — его слот освобождается
        // без анимации (ghost показывает элемент у пальца).
        visible: !del.isDragged
        color: pressArea.pressed ? "#d0d1d2" : "#e0e1e2"

        Item {
            id: iconBox
            width: 64
            height: 64
            anchors {
                left: parent.left
                leftMargin: 16
                verticalCenter: parent.verticalCenter
            }
            // Индикатор СЗАДИ иконки (medium = 64): пока иконка полного размера —
            // скрыт за ней; при выполнении иконка уменьшается и открывает его.
            // Тип: busy без процентов (в т.ч. Queued), progress — когда есть проценты.
            ExtendedIndicator {
                anchors.centerIn: parent
                size: "medium"
                type: model.indeterminate ? "busy" : "progress"
                progress: model.progress
                running: model.status === InstallationQueue.Running
                visible: model.status === InstallationQueue.Running
            }
            Image {
                id: iconImage
                // При выполнении иконка уменьшается (~40px), оставаясь квадратной,
                // и открывает индикатор за собой.
                width: model.status === InstallationQueue.Running ? 40 : 64
                height: width
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: model.iconUrl ? model.iconUrl : "image://theme/icon-m-content-ovi-store-inverse"
                Behavior on width { NumberAnimation { duration: 350; easing.type: Easing.InOutQuad } }
            }
        }

        Column {
            anchors {
                left: iconBox.right
                leftMargin: 16
                right: actions.left
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }
            spacing: 2

            Text {
                width: parent.width
                text: model.name
                color: "black"
                font.pixelSize: 24
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text: "By: " + model.developer
                color: "#666666"
                font.pixelSize: 18
                elide: Text.ElideRight
            }
            Row {
                spacing: 2
                Text {
                    id: statusText
                    font.pixelSize: 18
                    elide: Text.ElideRight
                    text: model.status === InstallationQueue.Done   ? "Completed"
                        : model.status === InstallationQueue.Failed ? "Error"
                        : model.status === InstallationQueue.Running ? (model.action ? model.action : "Working…")
                        : ("Queued. Action: " + del.__opName(model.operation))
                    color: model.status === InstallationQueue.Done   ? "#3a9d23"
                         : model.status === InstallationQueue.Failed ? "#cc0000"
                         : "#888888"
                }
                Image {
                    anchors.verticalCenter: statusText.verticalCenter
                    width: 26
                    height: 26
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                    visible: model.status === InstallationQueue.Done || model.status === InstallationQueue.Failed
                    source: model.status === InstallationQueue.Done ? "image://theme/icon-m-toolbar-done"
                          : "image://theme/icon-s-error"
                }
            }
        }

        // Маленькие кнопки-иконки справа: равные отступы сверху / между / снизу.
        Column {
            id: actions
            anchors {
                right: parent.right
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }
            // (высота - две иконки) / 3 промежутка => равные зазоры при двух кнопках,
            // при одной кнопке она просто центрируется по вертикали.
            spacing: (del.itemHeight - 2 * 40) / 3

            // Удалить из списка: активна, когда задача не выполняется
            // (в очереди или завершилась — успешно или с ошибкой).
            Item {
                width: 40
                height: 40
                opacity: (model.status === InstallationQueue.Running) ? 0.3 : 1.0
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-delete"
                }
                MouseArea {
                    anchors.fill: parent
                    enabled: model.status !== InstallationQueue.Running && del.state !== "removing"
                    onClicked: del.state = "removing"
                }
            }

            // Просмотр лога apt: у всех, кто запускался — во время установки,
            // после успешного завершения и после ошибки (нет лога только у Queued).
            Item {
                width: 40
                height: 40
                visible: model.status !== InstallationQueue.Queued
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-list"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: del.ListView.view.showLog(model.logModel)
                }
            }
        }

        // Разделитель между делегатами.
        Rectangle {
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            height: 1
            color: "#c6c7c8"
        }
    }

    // Долгий тап начинает перетаскивание; модель меняется только при отпускании.
    // Якорится к корню делегата (не к содержимому), поэтому остаётся на месте
    // даже когда содержимое источника скрыто, и захват мыши не теряется.
    MouseArea {
        id: pressArea
        anchors.fill: parent
        z: -1
        property bool dragging: false

        onPressAndHold: {
            // Двигать можно только ожидающие (ещё ни разу не запускавшиеся) задачи.
            if (model.status !== InstallationQueue.Queued)
                return;
            dragging = true;
            var p = mapToItem(del.ListView.view, mouseX, mouseY);
            del.ListView.view.startReorder(index, model.iconUrl, model.name, model.developer,
                                           statusText.text, statusText.color, p.y);
        }
        // Обычный тап (не перетаскивание) — открыть страницу приложения.
        // Тап по кнопкам (удалить/лог) перехватывается их MouseArea и сюда не доходит.
        onClicked: {
            // Открываем AppPage только для реальных id openrepos. Плейсхолдеры (<0) и
            // синтетические id удалений установленных приложений (>= 1e9) детальной
            // страницы не имеют.
            if (model.appId > 0 && model.appId < 1000000000)
                del.ListView.view.openApp(model.appId, iconImage);
        }
        onPositionChanged: {
            if (!dragging) return;
            var p = mapToItem(del.ListView.view, mouseX, mouseY);
            del.ListView.view.updateReorder(p.y);
        }
        onReleased: {
            if (!dragging) return;
            dragging = false;
            del.ListView.view.endReorder();
        }
        onCanceled: {
            if (!dragging) return;
            dragging = false;
            del.ListView.view.endReorder();
        }
    }
}
