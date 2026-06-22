import QtQuick 1.1
import com.nokia.meego 1.0
import "../components/delegates"
import "../components/header"
import "../components/ui"
import "../js/UIConstants.js" as UI

// Список приложений одного разработчика (GET /users/{uid}/apps). Открывается с
// AppPage по тапу на имя разработчика. Использует общий api.userModel и AppDelegate.
Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property int userId: -1
    property string developerName: ""
    property bool userRotating: false

    // Повтор загрузки из общего диалога ошибки (main.qml).
    function retry() {
        if (page.userId > 0) {
            waiter.show();
            api.getUserApps(page.userId);
            api.getUserProfile(page.userId);
        }
    }
    // Полное имя из профиля (real_name + real_surname), отбрасывая пустое и "None".
    function fullName() {
        var p = api.userProfile;
        if (!p) return "";
        var n = (p.real_name && p.real_name !== "None") ? p.real_name : "";
        var s = (p.real_surname && p.real_surname !== "None") ? p.real_surname : "";
        return (n + " " + s).replace(/\s+/g, " ").trim();
    }
    property string title: "<b>MeeShop</b>: Developer"

    property Component pageHeader: Component {
        Header { title: page.title }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop()
        }
    }

    Component.onCompleted: {
        if (page.userId > 0) {
            waiter.show();
            api.getUserApps(page.userId);
            api.getUserProfile(page.userId);
        }
    }

    // Шапка списка: карточка профиля (справа аватар, слева ник / имя / подпись) и
    // заголовок «<ник>'s apps». Обновляется реактивно по api.userProfile.
    Component {
        id: devHeader
        Column {
            width: list.width
            Item {
                width: parent.width
                height: 124
                // Плейсхолдер «человек» (силуэт на сквиркле), когда у автора нет фото
                // ИЛИ если фото не удалось декодировать.
                FallbackImage {
                    id: avatar
                    width: 96; height: 96
                    anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                    source: (api.userProfile.picture && api.userProfile.picture.url)
                            ? api.userProfile.picture.url : ""
                    fallbackSource: "image://theme/icon-l-sharing-avatar-placeholder"
                }
                Column {
                    anchors {
                        left: parent.left; leftMargin: 16
                        right: avatar.left; rightMargin: 16
                        verticalCenter: parent.verticalCenter
                    }
                    spacing: 4
                    Text {
                        width: parent.width
                        text: page.developerName
                        color: "black"; font.pixelSize: 26; font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        width: parent.width
                        text: page.fullName()
                        visible: text.length > 0
                        color: "#555555"; font.pixelSize: 20
                        elide: Text.ElideRight
                    }
                    Text {
                        width: parent.width
                        text: api.userProfile.signature ? api.userProfile.signature : ""
                        visible: text.length > 0
                        color: "#777777"; font.pixelSize: 18
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }
                }
            }
            SectionHeader { text: page.developerName + "'s apps" }
        }
    }

    Text {
        anchors.centerIn: parent
        text: "No apps"
        color: "#888888"
        font.pixelSize: 20
        // Во время загрузки список пуст, но сверху лежит Waiter — он перекрывает.
        visible: api.userModel.count === 0
    }

    ListView {
        id: list
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
        cacheBuffer: 1000
        model: api.userModel
        header: devHeader
        delegate: AppDelegate {
            onClicked: appWindow.pageStack.push(appPage, { appId: appId, gradientSource: iconImage, pushed: true })
        }
        footer: LoadMoreRectangle {
            id: footerItem
            width: list.width
            height: api.userHasMore ? 90 : 0
            opacity: api.userHasMore ? 1 : 0
            Behavior on height { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
            Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
            SectionHeader {}
            Connections {
                target: page
                onUserRotatingChanged: page.userRotating ? footerItem.startRotation() : footerItem.stopRotation()
            }
        }
        onAtYEndChanged: {
            if (atYEnd && api.userHasMore) {
                page.userRotating = true;
                api.loadMoreUser();
            }
        }
    }

    Waiter {
        id: waiter
        Connections {
            target: api
            onUserFinished: {
                page.userRotating = false;
                if (waiter.opacity == 1.0) waiter.hide();
            }
        }
    }
}
