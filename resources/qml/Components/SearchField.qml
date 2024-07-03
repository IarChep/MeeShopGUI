import QtQuick 1.1
import com.nokia.meego 1.1
import "UIConstants.js" as UI
import "EditBubble.js" as Popup
import "TextAreaHelper.js" as TextAreaHelper
import "Magnifier.js" as MagnifierPopup
import "SelectionHandles.js" as SelectionHandles

FocusScope {
    id: root

    // Common public API
    property alias text: textInput.text
    property alias placeholderText: prompt.text

    property alias inputMethodHints: textInput.inputMethodHints
    property alias font: textInput.font
    property alias cursorPosition: textInput.cursorPosition
    property alias maximumLength: textInput.maximumLength
    property alias readOnly: textInput.readOnly
    property alias acceptableInput: textInput.acceptableInput
    property alias inputMask: textInput.inputMask
    property alias validator: textInput.validator

    property alias selectedText: textInput.selectedText
    property alias selectionStart: textInput.selectionStart
    property alias selectionEnd: textInput.selectionEnd

    property alias echoMode: textInput.echoMode // ### TODO: declare own enum { Normal, Password }

    property bool errorHighlight: !acceptableInput
    // Property enableSoftwareInputPanel is DEPRECATED
    property alias enableSoftwareInputPanel: textInput.activeFocusOnPress

    property Item platformSipAttributes

    property bool platformEnableEditBubble: true

    property QtObject platformStyle: TextFieldStyle {}

    property alias style: root.platformStyle

    property Component customSoftwareInputPanel

    property Component platformCustomSoftwareInputPanel

    property alias platformPreedit: inputMethodObserver.preedit

    //force a western numeric input panel even when vkb is set to arabic
    property alias platformWesternNumericInputEnforced: textInput.westernNumericInputEnforced
    property bool platformSelectable: true

    signal accepted

    onPlatformSipAttributesChanged: {
        platformSipAttributes.registerInputElement(textInput)
    }

    onCustomSoftwareInputPanelChanged: {
        console.log("TextField's property customSoftwareInputPanel is deprecated. Use property platformCustomSoftwareInputPanel instead.")
        platformCustomSoftwareInputPanel = customSoftwareInputPanel
    }

    onPlatformCustomSoftwareInputPanelChanged: {
        textInput.activeFocusOnPress = platformCustomSoftwareInputPanel == null
    }



    function copy() {
        textInput.copy()
    }

    Connections {
        target: platformWindow

        onActiveChanged: {
            if(platformWindow.active) {
                if (__hadFocusBeforeMinimization) {
                    __hadFocusBeforeMinimization = false
                    if (root.parent)
                        root.focus = true
                    else
                        textInput.focus = true
                }

                if (!readOnly) {
                    if (activeFocus) {
                        if (platformCustomSoftwareInputPanel != null) {
                            platformOpenSoftwareInputPanel();
                        } else {
                            inputContext.simulateSipOpen();
                        }
                        repositionTimer.running = true;
                    }
                }
            } else {
                if (activeFocus) {
                    platformCloseSoftwareInputPanel();
                    Popup.close(textInput);
                    SelectionHandles.close(textInput);
                    if (textInput.selectionStart != textInput.selectionEnd)
                        textInput.deselect();

                    __hadFocusBeforeMinimization = true
                    if (root.parent)
                        root.parent.focus = true
                    else
                        textInput.focus = false
                }
            }
        }

        onAnimatingChanged: {
            if (!platformWindow.animating && root.activeFocus) {
                TextAreaHelper.repositionFlickable(contentMovingAnimation);
            }
        }
    }


    function paste() {
        textInput.paste()
    }

    function cut() {
        textInput.cut()
    }

    function select(start, end) {
        textInput.select(start, end)
    }

    function selectAll() {
        textInput.selectAll()
    }

    function selectWord() {
        textInput.selectWord()
    }

    function positionAt(x) {
        var p = mapToItem(textInput, x, 0);
        return textInput.positionAt(p.x)
    }

    function positionToRectangle(pos) {
        var rect = textInput.positionToRectangle(pos)
        rect.x = mapFromItem(textInput, rect.x, 0).x
        return rect;
    }

    // ensure propagation of forceActiveFocus
    function forceActiveFocus() {
        textInput.forceActiveFocus()
    }

    function closeSoftwareInputPanel() {
        console.log("TextField's function closeSoftwareInputPanel is deprecated. Use function platformCloseSoftwareInputPanel instead.")
        platformCloseSoftwareInputPanel()
    }

    function platformCloseSoftwareInputPanel() {
        inputContext.simulateSipClose();
        if (inputContext.customSoftwareInputPanelVisible) {
            inputContext.customSoftwareInputPanelVisible = false
            inputContext.customSoftwareInputPanelComponent = null
            inputContext.customSoftwareInputPanelTextField = null
        } else {
            textInput.closeSoftwareInputPanel();
        }
    }

    function openSoftwareInputPanel() {
        console.log("TextField's function openSoftwareInputPanel is deprecated. Use function platformOpenSoftwareInputPanel instead.")
        platformOpenSoftwareInputPanel()
    }

    function platformOpenSoftwareInputPanel() {
        inputContext.simulateSipOpen();
        if (platformCustomSoftwareInputPanel != null && !inputContext.customSoftwareInputPanelVisible) {
            inputContext.customSoftwareInputPanelTextField = root
            inputContext.customSoftwareInputPanelComponent = platformCustomSoftwareInputPanel
            inputContext.customSoftwareInputPanelVisible = true
        } else {
            textInput.openSoftwareInputPanel();
        }
    }

    // private
    property bool __expanding: true // Layout hint used but ToolBarLayout
    property int __preeditDisabledMask: Qt.ImhHiddenText|
                                        Qt.ImhNoPredictiveText|
                                        Qt.ImhDigitsOnly|
                                        Qt.ImhFormattedNumbersOnly|
                                        Qt.ImhDialableCharactersOnly|
                                        Qt.ImhEmailCharactersOnly|
                                        Qt.ImhUrlCharactersOnly

    property bool __hadFocusBeforeMinimization: false

    implicitWidth: platformStyle.defaultWidth
    implicitHeight: UI.FIELD_DEFAULT_HEIGHT

    onActiveFocusChanged: {
        if (!readOnly) {
            if (activeFocus) {
                if (platformCustomSoftwareInputPanel != null) {
                    platformOpenSoftwareInputPanel();
                } else {
                    inputContext.simulateSipOpen();
                }

                repositionTimer.running = true;
            } else {
                platformCloseSoftwareInputPanel();
                Popup.close(textInput);
                SelectionHandles.close(textInput);
            }
        }

        if (!activeFocus)
            MagnifierPopup.close();
    }


    BorderImage {
        id: background
        source: errorHighlight?
            platformStyle.backgroundError:
            readOnly?
            platformStyle.backgroundDisabled:
        textInput.activeFocus?
            platformStyle.backgroundSelected:
            platformStyle.background

        anchors.fill: parent
        border.left: root.platformStyle.backgroundCornerMargin; border.top: root.platformStyle.backgroundCornerMargin
        border.right: root.platformStyle.backgroundCornerMargin; border.bottom: root.platformStyle.backgroundCornerMargin
    }

    Text {
        id: prompt

        anchors {verticalCenter: root.verticalCenter; left: textInput.left; right: parent.right}
        anchors.rightMargin: root.platformStyle.paddingRight
        anchors.verticalCenterOffset: root.platformStyle.baselineOffset

        font: root.platformStyle.textFont
        color: root.platformStyle.promptTextColor
        elide: Text.ElideRight

        // opacity for default state
        opacity: 0.0

        states: [
            State {
                name: "unfocused"
                // memory allocation optimization: cursorPosition is checked to minimize displayText evaluations
                when: !root.activeFocus && textInput.cursorPosition == 0 && !textInput.text && prompt.text && !textInput.inputMethodComposing
                PropertyChanges { target: prompt; opacity: 1.0; }
            },
            State {
                name: "focused"
                // memory allocation optimization: cursorPosition is checked to minimize displayText evaluations
                when: root.activeFocus && textInput.cursorPosition == 0 && !textInput.text && prompt.text && !textInput.inputMethodComposing
                PropertyChanges { target: prompt; opacity: 0.6; }
            }
        ]

        transitions: [
            Transition {
                from: "unfocused"; to: "focused";
                reversible: true
                SequentialAnimation {
                    PauseAnimation { duration: 60 }
                    NumberAnimation { target: prompt; properties: "opacity"; duration: 150  }
                }
            },
            Transition {
                from: "focused"; to: "";
                reversible: true
                SequentialAnimation {
                    PauseAnimation { duration:  60 }
                    NumberAnimation { target: prompt; properties: "opacity"; duration: 100 }
                }
            }
        ]
    }

    MouseArea {
        enabled: !textInput.activeFocus
        z: enabled?1:0
        anchors.fill: parent
        anchors.margins: UI.TOUCH_EXPANSION_MARGIN
        onClicked: {
            if (!textInput.activeFocus) {
                textInput.forceActiveFocus();

                // activate to preedit and/or move the cursor
                var preeditDisabled = root.inputMethodHints &
                                      root.__preeditDisabledMask
                var injectionSucceeded = false;
                var newCursorPosition = textInput.positionAt(mapToItem(textInput, mouseX, mouseY).x,TextInput.CursorOnCharacter);
                if (!preeditDisabled) {
                    var beforeText = textInput.text
                    if (!TextAreaHelper.atSpace(newCursorPosition, beforeText)
                        && newCursorPosition != beforeText.length
                        && !(newCursorPosition == 0 || TextAreaHelper.atSpace(newCursorPosition - 1, beforeText))) {

                        injectionSucceeded = TextAreaHelper.injectWordToPreedit(newCursorPosition, beforeText);
                    }
                }
                if (!injectionSucceeded) {
                    textInput.cursorPosition=newCursorPosition;
                }
            }
        }
    }
    Image {
        id: searchImg
        anchors.left: root.left
        anchors.leftMargin: root.platformStyle.paddingLeft
        anchors.verticalCenter: root.verticalCenter
        source: "image://theme/icon-l-search-main-view"
    }

    TextInput {
        id: textInput

        property alias preedit: inputMethodObserver.preedit
        property alias preeditCursorPosition: inputMethodObserver.preeditCursorPosition

        // this properties are evaluated by the input method framework
        property bool westernNumericInputEnforced: false
        property bool suppressInputMethod: !activeFocusOnPress

        onWesternNumericInputEnforcedChanged: {
            inputContext.update();
        }

        anchors {verticalCenter: parent.verticalCenter; left: searchImg.right; right: parent.right}
        anchors.leftMargin: root.platformStyle.paddingLeft
        anchors.rightMargin: root.platformStyle.paddingRight
        anchors.verticalCenterOffset: root.platformStyle.baselineOffset

        passwordCharacter: "\u2022"
        font: root.platformStyle.textFont
        color: root.platformStyle.textColor
        selectByMouse: false
        selectedTextColor: root.platformStyle.selectedTextColor
        selectionColor: root.platformStyle.selectionColor
        mouseSelectionMode: TextInput.SelectWords
        focus: true

        onAccepted: { root.accepted() }

        Component.onDestruction: {
            SelectionHandles.close(textInput);
            Popup.close(textInput);
        }

        Connections {
            target: TextAreaHelper.findFlickable(root.parent)

            onContentYChanged: if (root.activeFocus) TextAreaHelper.filteredInputContextUpdate();
            onContentXChanged: if (root.activeFocus) TextAreaHelper.filteredInputContextUpdate();
            onMovementEnded: inputContext.update();
        }

        Connections {
            target: inputContext

            onSoftwareInputPanelRectChanged: {
                if (activeFocus) {
                    repositionTimer.running = true
                }
            }
        }

        onTextChanged: {
            if(root.activeFocus) {
                TextAreaHelper.repositionFlickable(contentMovingAnimation)
            }

            if (Popup.isOpened(textInput) && !Popup.isChangingInput())
                Popup.close(textInput);
            SelectionHandles.close(textInput);
        }

        onCursorPositionChanged: {
            if (MagnifierPopup.isOpened() &&
                Popup.isOpened()) {
                Popup.close(textInput);
            } else if (!mouseFilter.attemptToActivate ||
                textInput.cursorPosition == textInput.text.length) {
                if ( Popup.isOpened(textInput) &&
                !Popup.isChangingInput()) {
                    Popup.close(textInput);
                    Popup.open(textInput,
                        textInput.positionToRectangle(textInput.cursorPosition));
                }
                if ( SelectionHandles.isOpened(textInput) && textInput.selectedText == "") {
                    SelectionHandles.close( textInput );
                }
                if ( !SelectionHandles.isOpened(textInput) && textInput.selectedText != ""
                     && platformEnableEditBubble == true ) {
                    SelectionHandles.open( textInput );
                }
                SelectionHandles.adjustPosition();
            }
        }

        onSelectedTextChanged: {
            if ( !platformSelectable )
                textInput.deselect(); // enforce deselection in all cases we didn't think of

            if (Popup.isOpened(textInput) && !Popup.isChangingInput()) {
                Popup.close(textInput);
            }
            if ( SelectionHandles.isOpened(textInput) && textInput.selectedText == "") {
                SelectionHandles.close( textInput )
            }
        }

        InputMethodObserver {
            id: inputMethodObserver

            onPreeditChanged: {
                if(root.activeFocus) {
                    TextAreaHelper.repositionFlickable(contentMovingAnimation)
                }

                if (Popup.isOpened(textInput) && !Popup.isChangingInput()) {
                    Popup.close(textInput);
                }
            }
        }

        Timer {
            id: repositionTimer
            interval: 350
            onTriggered: {
                TextAreaHelper.repositionFlickable(contentMovingAnimation)
            }
        }

        PropertyAnimation {
            id: contentMovingAnimation
            property: "contentY"
            duration: 200
            easing.type: Easing.InOutCubic
        }

        MouseFilter {
            id: mouseFilter
            anchors.fill: parent
            anchors.leftMargin:  UI.TOUCH_EXPANSION_MARGIN - root.platformStyle.paddingLeft
            anchors.rightMargin:  UI.TOUCH_EXPANSION_MARGIN - root.platformStyle.paddingRight
            anchors.topMargin: UI.TOUCH_EXPANSION_MARGIN - ((root.height - parent.height) / 2)
            anchors.bottomMargin:  UI.TOUCH_EXPANSION_MARGIN - ((root.height - parent.height) / 2)

            property bool attemptToActivate: false
            property bool pressOnPreedit: false
            property int oldCursorPosition: 0

            property variant editBubblePosition: null

            onPressed: {
                var mousePosition = textInput.positionAt(mouse.x,TextInput.CursorOnCharacter);
                pressOnPreedit = textInput.cursorPosition==mousePosition
                oldCursorPosition = textInput.cursorPosition;
                var preeditDisabled = root.inputMethodHints &
                                      root.__preeditDisabledMask

                attemptToActivate = !pressOnPreedit && !root.readOnly && !preeditDisabled && root.activeFocus &&
                                    !(mousePosition == 0 || TextAreaHelper.atSpace(mousePosition - 1) || TextAreaHelper.atSpace(mousePosition));
                mouse.filtered = true;
            }

            onDelayedPressSent: {
                if (textInput.preedit) {
                    textInput.cursorPosition = oldCursorPosition;
                }
            }

            onHorizontalDrag: {
                // possible pre-edit word have to be commited before selection
                if (root.activeFocus || root.readOnly) {
                    inputContext.reset()
                    if( platformSelectable )
                        parent.selectByMouse = true
                    attemptToActivate = false
                }
            }

            onPressAndHold:{
                // possible pre-edit word have to be commited before showing the magnifier
                if ((root.text != "" || inputMethodObserver.preedit != "") && root.activeFocus) {
                    inputContext.reset()
                    attemptToActivate = false
                    MagnifierPopup.open(root);
                    var magnifier = MagnifierPopup.popup;
                    var cursorPos = textInput.positionToRectangle(0);
                    var mappedPosMf = mapFromItem(parent,mouse.x,cursorPos.y+cursorPos.height/2+4);
                    magnifier.xCenter = mapToItem(magnifier.sourceItem,mappedPosMf.x,0).x;
                    var mappedPos =  mapToItem(magnifier.parent, mappedPosMf.x - magnifier.width / 2,
                                               textInput.y - 120 - UI.MARGIN_XLARGE - (height / 2));
                    var yAdjustment = -mapFromItem(magnifier.__rootElement, 0, 0).y < magnifier.height / 2.5 ? magnifier.height / 2.5 + mapFromItem(magnifier.__rootElement, 0,0).y : 0
                    magnifier.x = mappedPos.x;
                    magnifier.y = mappedPos.y + yAdjustment;
                    magnifier.yCenter = mapToItem(magnifier.sourceItem,0,mappedPosMf.y).y;
                    parent.cursorPosition = textInput.positionAt(mouse.x)
                }
            }

            onReleased: {
                if (MagnifierPopup.isOpened()) {
                    MagnifierPopup.close();
                }

                if (attemptToActivate)
                    inputContext.reset();

                var newCursorPosition = textInput.positionAt(mouse.x,TextInput.CursorOnCharacter);
                if (textInput.preedit.length == 0)
                    editBubblePosition = textInput.positionToRectangle(newCursorPosition);

                if (attemptToActivate) {
                    var beforeText = textInput.text;

                    textInput.cursorPosition = newCursorPosition;
                    var injectionSucceeded = false;

                    if (!TextAreaHelper.atSpace(newCursorPosition, beforeText)
                             && newCursorPosition != beforeText.length) {
                        injectionSucceeded = TextAreaHelper.injectWordToPreedit(newCursorPosition, beforeText);
                    }
                    if (injectionSucceeded) {
                        mouse.filtered=true;
                        if (textInput.preedit.length >=1 && textInput.preedit.length <= 4)
                            editBubblePosition = textInput.positionToRectangle(textInput.cursorPosition+1)
                    } else {
                        textInput.text=beforeText;
                        textInput.cursorPosition=newCursorPosition;
                    }
                } else if (!parent.selectByMouse) {
                    if (!pressOnPreedit) inputContext.reset();
                    textInput.cursorPosition = textInput.positionAt(mouse.x,TextInput.CursorOnCharacter);
                }
                parent.selectByMouse = false;
            }

            onFinished: {
                if (root.activeFocus && platformEnableEditBubble) {
                    if (textInput.preedit.length == 0)
                        editBubblePosition = textInput.positionToRectangle(textInput.cursorPosition);
                    if (editBubblePosition != null) {
                        Popup.open(textInput,editBubblePosition);
                        editBubblePosition = null
                    }
                    if (textInput.selectedText != "")
                        SelectionHandles.open( textInput );
                    SelectionHandles.adjustPosition();
                }
                attemptToActivate = false
            }

            onMousePositionChanged: {
                if (MagnifierPopup.isOpened() && !parent.selectByMouse) {
                    textInput.cursorPosition = textInput.positionAt(mouse.x)
                    var magnifier = MagnifierPopup.popup;
                    var mappedPosMf = mapFromItem(parent,mouse.x,0);
                    var mappedPos =  mapToItem(magnifier.parent,mappedPosMf.x - magnifier.width / 2.0, 0);
                    magnifier.xCenter = mapToItem(magnifier.sourceItem,mappedPosMf.x,0).x;
                    magnifier.x = mappedPos.x;
                }
                SelectionHandles.adjustPosition();
            }

            onDoubleClicked: {
                // possible pre-edit word have to be commited before selection
                inputContext.reset()
                if( platformSelectable )
                    parent.selectByMouse = true
                attemptToActivate = false
            }
        }
    }

    InverseMouseArea {
        anchors.fill: parent
        anchors.margins: UI.TOUCH_EXPANSION_MARGIN
        enabled: textInput.activeFocus
        onClickedOutside: {
            if (Popup.isOpened(textInput) && ((mouseX > Popup.geometry().left && mouseX < Popup.geometry().right) &&
                                           (mouseY > Popup.geometry().top && mouseY < Popup.geometry().bottom))) {
                return;
            }
            root.parent.focus = true;
        }
    }
}
