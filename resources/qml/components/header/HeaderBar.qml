import QtQuick 1.1
import "../../js/UIConstants.js" as Ui

// Единый глобальный заголовок (аналог платформенного ToolBar, но сверху).
// Два независимых слоя:
//   * ФОН (background) — отдельный объект (Rectangle/Image). Не масштабируется и
//     не сдвигается; меняется лёгким растворением (кроссфейд) и ТОЛЬКО когда он
//     действительно другой.
//   * СОДЕРЖИМОЕ (content) — дети заголовка (заголовок-текст, кнопки). Меняется с
//     теми же анимациями, что и у тулбара (push/pop/replace/set).
// И фон, и содержимое — Component'ы: они инстанцируются здесь, но их привязки и
// MouseArea работают в контексте своей страницы (активны лишь пока она текущая).
Item {
    id: root

    width: parent ? parent.width : 0
    clip: true

    property Component content: null
    property Component background: null

    // Длительности. Сворачивание высоты держим равным смене содержимого, чтобы при
    // уходе на страницу без заголовка всё заканчивалось одновременно (без обрезки).
    property int contentTransitionDuration: 400
    property int visibilityTransitionDuration: 400
    property int backgroundTransitionDuration: 400

    // Высота: полная пока есть что показывать, иначе 0 (сворачивается, как тулбар).
    states: [
        State {
            name: "hidden"
            when: content == null && background == null
            PropertyChanges { target: root; height: 0 }
        },
        State {
            name: ""
            when: !(content == null && background == null)
            PropertyChanges { target: root; height: Ui.HEADER_DEFAULT_HEIGHT_PORTRAIT }
        }
    ]
    transitions: Transition {
        from: ""; to: "hidden"; reversible: true
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutExpo; duration: root.visibilityTransitionDuration }
    }

    // Слой фона (снизу). Постоянная высота — не сжимается при сворачивании root.
    Item {
        id: bgLayer
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: Ui.HEADER_DEFAULT_HEIGHT_PORTRAIT
    }

    // Слой содержимого (сверху). Контейнеры живут в хосте постоянной высоты (как у
    // ToolBar — там это bgImage), чтобы содержимое не сжималось при сворачивании.
    Item {
        id: contentHost
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: Ui.HEADER_DEFAULT_HEIGHT_PORTRAIT
    }

    // Задаёт фон и содержимое сразу. transition — тип перехода содержимого.
    function setHeader(newContent, newBackground, transition) {
        __transition = transition;
        root.background = newBackground;   // onBackgroundChanged (только если другой)
        root.content = newContent;          // onContentChanged
        __transition = undefined;
    }

    property variant __transition

    onBackgroundChanged: __swapBackground()
    onContentChanged: __performTransition(__transition || "set")

    // ---------------------------------------------------------- ФОН (кроссфейд)
    property Item __bgA: null
    property Item __bgB: null
    property Item __bgCurrent: null
    property bool __bgFirst: true

    function __swapBackground() {
        if (!__bgCurrent) {
            __bgA = bgSlotComponent.createObject(bgLayer);
            __bgB = bgSlotComponent.createObject(bgLayer);
            __bgCurrent = __bgB;
        }
        // Не анимируем, если фон не изменился.
        if (__bgCurrent.src === background)
            return;

        var slot = (__bgCurrent === __bgA) ? __bgB : __bgA;
        slot.src = background;
        if (background) {
            // Самый первый показ — мгновенный: растворение играет только при
            // РЕАЛЬНОЙ смене фона, а не как проявление «из ничего» на старте.
            // (opacity=1 до смены состояния делает переход shown 1->1 без анимации.)
            if (__bgFirst)
                slot.opacity = 1;
            slot.state = "shown";
        } else {
            slot.state = "hidden";
        }
        __bgCurrent.state = "hidden";
        __bgCurrent = slot;
        __bgFirst = false;
    }

    Component {
        id: bgSlotComponent
        Item {
            id: bgSlot
            anchors.fill: parent
            opacity: 0
            state: "hidden"

            property Component src: null
            property Item inst: null

            onSrcChanged: {
                if (inst) { inst.destroy(); inst = null; }
                if (src)
                    inst = src.createObject(bgSlot);
            }

            states: [
                State { name: "shown";  PropertyChanges { target: bgSlot; opacity: 1 } },
                State { name: "hidden"; PropertyChanges { target: bgSlot; opacity: 0 } }
            ]
            transitions: [
                Transition {
                    to: "shown"
                    NumberAnimation { properties: "opacity"; duration: root.backgroundTransitionDuration; easing.type: Easing.InOutQuad }
                },
                Transition {
                    to: "hidden"
                    SequentialAnimation {
                        NumberAnimation { properties: "opacity"; duration: root.backgroundTransitionDuration; easing.type: Easing.InOutQuad }
                        ScriptAction { script: { if (bgSlot.inst) { bgSlot.inst.destroy(); bgSlot.inst = null; } bgSlot.src = null; } }
                    }
                }
            ]
        }
    }

    // ------------------------------------------------ СОДЕРЖИМОЕ (стиль ToolBar)
    property Item __currentContainer: null
    property Item __containerA: null
    property Item __containerB: null

    function __performTransition(transition) {
        if (!__currentContainer) {
            __containerA = containerComponent.createObject(contentHost);
            __containerB = containerComponent.createObject(contentHost);
            __currentContainer = __containerB;
        }

        if (__currentContainer.src === content)
            return;

        var transitions = {
            "set":     { "new": "",      "old": "hidden" },
            "push":    { "new": "right", "old": "left" },
            "pop":     { "new": "left",  "old": "right" },
            "replace": { "new": "front", "old": "back" }
        };
        var animation = transitions[transition] || transitions["set"];

        var container = (__currentContainer === __containerA) ? __containerB : __containerA;
        container.state = "hidden";
        container.src = content;

        __currentContainer.state = animation["old"];
        if (content) {
            container.state = animation["new"];
            container.state = "";
        }

        __currentContainer = container;
    }

    Component {
        id: containerComponent

        Item {
            id: container

            anchors.fill: parent
            state: "hidden"

            // Контент реагирует на ввод только когда контейнер «осел» в текущем
            // состоянии: уходящий заголовок перестаёт ловить тапы сразу при старте
            // перехода (тап по заголовку срабатывает лишь пока страница текущая).
            enabled: state == ""

            property Component src: null
            property Item inst: null

            onSrcChanged: {
                if (inst) { inst.destroy(); inst = null; }
                if (src)
                    inst = src.createObject(container);
            }

            states: [
                State { name: "left";  PropertyChanges { target: container; x: -30; opacity: 0.0 } },
                State { name: "right"; PropertyChanges { target: container; x: 30;  opacity: 0.0 } },
                State { name: "front"; PropertyChanges { target: container; scale: 1.25; opacity: 0.0 } },
                State { name: "back";  PropertyChanges { target: container; scale: 0.85; opacity: 0.0 } },
                State {
                    name: "hidden"
                    PropertyChanges { target: container; visible: false }
                    StateChangeScript {
                        script: {
                            if (container.inst) {
                                container.inst.destroy();
                                container.inst = null;
                            }
                            container.src = null;
                        }
                    }
                }
            ]

            transitions: [
                Transition {
                    from: ""; to: "left"; reversible: true
                    SequentialAnimation {
                        PropertyAnimation { properties: "x,opacity"; easing.type: Easing.InCubic; duration: root.contentTransitionDuration / 2 }
                        PauseAnimation { duration: root.contentTransitionDuration / 2 }
                        ScriptAction { script: if (state == "left") state = "hidden"; }
                    }
                },
                Transition {
                    from: ""; to: "right"; reversible: true
                    SequentialAnimation {
                        PropertyAnimation { properties: "x,opacity"; easing.type: Easing.InCubic; duration: root.contentTransitionDuration / 2 }
                        PauseAnimation { duration: root.contentTransitionDuration / 2 }
                        ScriptAction { script: if (state == "right") state = "hidden"; }
                    }
                },
                Transition {
                    from: "front"; to: "";
                    PropertyAnimation { properties: "scale,opacity"; easing.type: Easing.InOutExpo; duration: root.contentTransitionDuration }
                },
                Transition {
                    from: ""; to: "back";
                    SequentialAnimation {
                        PropertyAnimation { properties: "scale,opacity"; easing.type: Easing.InOutExpo; duration: root.contentTransitionDuration }
                        ScriptAction { script: if (state == "back") state = "hidden"; }
                    }
                }
            ]
        }
    }
}
