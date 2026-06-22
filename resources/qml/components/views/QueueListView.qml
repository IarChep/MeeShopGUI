import QtQuick 1.1
import com.nokia.meego 1.0
import "../../js/UIConstants.js" as UI
import "../delegates"
import "../sheets"

// Список очереди установок с перетаскиванием — самодостаточный блок (список +
// затемнение + «поднятый» ghost + лист лога + подсказка снизу). Используется и
// боевой страницей (InstallationQueuePage), и тестовой (TestQueuePage), чтобы
// логика drag/clamp была в ОДНОМ месте. Работает с контекст-объектом `queue`.
Item {
    id: root

    property bool reordering: false
    // Фаза «падения» ghost в открытый зазор после отпускания (список при этом уже
    // выезжает вперёд, а зазор ещё держится открытым, пока ghost не приземлится).
    property bool dropping: false
    property int dragIndex: -1
    property int dropTarget: -1
    property int itemHeight: 100 // высота QueueDelegate

    // Тап по делегату (не перетаскивание) — открыть страницу приложения.
    // Обрабатывается страницей (у неё есть доступ к Component appPage).
    signal appActivated(int appId, variant icon)

    // ghost по пальцу, ограниченный областью списка (над подсказкой снизу).
    function __ghostY(viewY) {
        var gy = queueList.mapToItem(root, 0, viewY).y - dragGhost.height / 2;
        if (gy < 0) gy = 0;
        var maxY = hintBar.y - dragGhost.height;
        if (gy > maxY) gy = maxY;
        return gy;
    }

    // Завершение «падения»: фиксируем модель и снимаем режим. Бесшовно — ghost уже
    // стоит точно над зазором (масштаб 1.0), а на его месте появляется реальный
    // элемент (сдвиги схлопываются в ту же позицию).
    function __commitDrop() {
        var from = root.dragIndex;
        var to = root.dropTarget;
        root.reordering = false;
        root.dropping = false;
        root.dragIndex = -1;
        root.dropTarget = -1;
        dragGhost.visible = false;
        if (from >= 0 && to >= 0 && from !== to)
            queue.move(from, to);
    }

    // Область списка (над подсказкой). При перетаскивании отъезжает назад; при
    // «падении» выезжает обратно вперёд — синхронно с приземлением ghost.
    Item {
        id: scaledGroup
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: hintBar.top }
        scale: (root.reordering && !root.dropping) ? 0.93 : 1.0
        Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

        Rectangle {
            anchors.fill: parent
            color: UI.COLOR_BACKGROUND

            Text {
                anchors.centerIn: parent
                text: "The queue is empty"
                font.pixelSize: 22
                color: "#888888"
                visible: queueList.count === 0
            }

            ListView {
                id: queueList
                anchors.fill: parent
                clip: true
                model: queue.model
                cacheBuffer: 2000
                interactive: !root.reordering
                delegate: QueueDelegate {}

                // Прокидываем состояние перетаскивания делегатам (ListView.view.*).
                property int dragIndex: root.dragIndex
                property int dropTarget: root.dropTarget

                // Открыть лог конкретного приложения.
                function showLog(logModel) {
                    logSheet.logModel = logModel;
                    logSheet.open();
                }

                // Тап по делегату — открыть страницу приложения (через сигнал страницы).
                function openApp(appId, icon) {
                    root.appActivated(appId, icon);
                }

                // --- Перетаскивание для смены порядка ---
                // viewY — позиция пальца в координатах ListView (вьюпорта).
                function startReorder(idx, icon, name, dev, statusStr, statusCol, viewY) {
                    // ВАЖЕН ПОРЯДОК: сначала dropTarget = idx, потом dragIndex = idx.
                    // Иначе при включении reordering (dragIndex>=0) dropTarget ещё был
                    // бы -1, и формула сдвига дала бы +itemHeight всем элементам выше
                    // перетаскиваемого => они бы дёрнулись. С этим порядком dropTarget
                    // уже == dragIndex => сдвиги = 0, список не двигается.
                    root.dropTarget = idx;
                    root.dragIndex = idx;
                    dragGhost.iconUrl = icon;
                    dragGhost.appName = name;
                    dragGhost.developer = dev;
                    dragGhost.statusStr = statusStr;
                    dragGhost.statusCol = statusCol;
                    // Ghost появляется НА МЕСТЕ реального элемента (бесшовно) и плавно
                    // «всплывает» к пальцу — зеркало анимации падения при отпускании.
                    dragGhost.y = idx * root.itemHeight - queueList.contentY;
                    dragGhost.visible = true;
                    root.reordering = true; // список отъезжает, ghost растёт (Behaviors)
                    riseAnim.to = root.__ghostY(viewY);
                    riseAnim.restart();
                }
                function updateReorder(viewY) {
                    if (root.dragIndex < 0 || root.dropping)
                        return;
                    // Палец двинулся — прекращаем «всплытие», дальше ghost следует за пальцем.
                    riseAnim.stop();
                    // Призрак следует за пальцем (в пределах списка).
                    dragGhost.y = root.__ghostY(viewY);
                    // Раскладка не искажается (двигаем content, не высоты) => contentY
                    // стабилен, цель считаем по исходным слотам.
                    var contentPos = viewY + queueList.contentY;
                    var t = Math.floor(contentPos / root.itemHeight);
                    // Не выше границы (ожидающее нельзя поставить выше
                    // выполняемых/завершённых). Границу берём ЖИВУЮ.
                    var minT = queue.firstQueuedIndex();
                    if (t < minT) t = minT;
                    if (t > count - 1) t = count - 1;
                    root.dropTarget = t;
                }
                function endReorder() {
                    if (root.dragIndex < 0 || root.dropping)
                        return;
                    // Прерываем «всплытие» (если отпустили сразу, не двигая палец) —
                    // иначе riseAnim и dropAnim одновременно тянут dragGhost.y и дёргают.
                    riseAnim.stop();
                    // Цель «падения» — открытый зазор (слот dropTarget) при масштабе 1.0.
                    dropYAnim.to = root.dropTarget * root.itemHeight - queueList.contentY;
                    root.dropping = true; // список выезжает вперёд, затемнение гаснет, ghost уменьшается
                    dropAnim.restart();   // ghost плавно «падает» в зазор; по завершении — __commitDrop
                }
            }
        }
    }

    // Затемнение (над подсказкой). Гаснет при «падении».
    Rectangle {
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: hintBar.top }
        color: "black"
        opacity: (root.reordering && !root.dropping) ? 0.45 : 0
        visible: opacity > 0
        z: 50
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    // «Поднятый» перетаскиваемый элемент. При отпускании плавно «падает» в открытый
    // зазор (dropAnim по Y + уменьшение масштаба 1.05->1.0), затем заменяется
    // реальным элементом списка (бесшовно — позиции совпадают).
    Rectangle {
        id: dragGhost
        property string iconUrl: ""
        property string appName: ""
        property string developer: ""
        property string statusStr: ""
        property color statusCol: "#888888"
        visible: false
        z: 100
        x: 0
        width: root.width
        height: root.itemHeight
        color: "#e0e1e2"
        border.color: "#b8b9ba"
        border.width: 1
        scale: (root.reordering && !root.dropping) ? 1.05 : 1.0
        Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

        Image {
            id: ghostIcon
            width: 64
            height: 64
            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
            fillMode: Image.PreserveAspectFit
            source: dragGhost.iconUrl ? dragGhost.iconUrl : "image://theme/icon-m-content-ovi-store-inverse"
        }
        Column {
            anchors {
                left: ghostIcon.right; leftMargin: 16
                right: parent.right; rightMargin: 16
                verticalCenter: parent.verticalCenter
            }
            spacing: 2
            Text { width: parent.width; text: dragGhost.appName; color: "black"; font.pixelSize: 24; elide: Text.ElideRight }
            Text { width: parent.width; text: "By: " + dragGhost.developer; color: "#666666"; font.pixelSize: 18; elide: Text.ElideRight }
            Text { width: parent.width; text: dragGhost.statusStr; color: dragGhost.statusCol; font.pixelSize: 18; elide: Text.ElideRight }
        }
    }

    // «Падение» ghost: едет по Y в открытый зазор, по завершении — коммит.
    // Длительность чуть БОЛЬШЕ, чем возврат масштаба (200мс): к моменту коммита
    // список уже на scale 1.0, поэтому ghost (в координатах root, без масштаба)
    // приземляется ровно туда, где появится реальный элемент — без рывка.
    SequentialAnimation {
        id: dropAnim
        NumberAnimation { id: dropYAnim; target: dragGhost; property: "y"; duration: 250; easing.type: Easing.OutCubic }
        ScriptAction { script: root.__commitDrop() }
    }

    // «Всплытие» ghost к пальцу при входе в режим — зеркало падения (та же плавность).
    NumberAnimation { id: riseAnim; target: dragGhost; property: "y"; duration: 250; easing.type: Easing.OutCubic }

    // Подсказка снизу: что делают кнопки и долгий тап.
    Rectangle {
        id: hintBar
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: 44
        color: "#d4d5d6"

        // Тонкий разделитель сверху.
        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 1
            color: "#b8b9ba"
        }

        // Все группы одной высоты (height: parent.height) + содержимое по
        // verticalCenter => «Hold — Reorder» (без иконки) выровнен по той же
        // горизонтали, что и группы иконка-текст (раньше он был выше).
        Row {
            anchors.centerIn: parent
            height: 28
            spacing: 22

            Row {
                height: parent.height
                spacing: 4
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 26; height: 26; smooth: true; fillMode: Image.PreserveAspectFit
                    source: "image://theme/icon-m-toolbar-delete"
                }
                Text { anchors.verticalCenter: parent.verticalCenter; text: "Cancel"; font.pixelSize: 16; color: "#3a3a3a" }
            }
            Row {
                height: parent.height
                spacing: 4
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 26; height: 26; smooth: true; fillMode: Image.PreserveAspectFit
                    source: "image://theme/icon-m-toolbar-list"
                }
                Text { anchors.verticalCenter: parent.verticalCenter; text: "Install log"; font.pixelSize: 16; color: "#3a3a3a" }
            }
            Row {
                height: parent.height
                spacing: 4
                Text { anchors.verticalCenter: parent.verticalCenter; text: "Hold"; font.pixelSize: 16; font.bold: true; color: "#3a3a3a" }
                Text { anchors.verticalCenter: parent.verticalCenter; text: "— Reorder"; font.pixelSize: 16; color: "#3a3a3a" }
            }
        }
    }

    AptLogSheet { id: logSheet }
}
