import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import IarChep.MeeShop 1.0

import "../components/delegates"
import "../components/header"
import "../components/ui"
import "../js/UIConstants.js" as UI

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title: "<b>MeeShop</b>: Home"
    property bool loadingRecent: false
    property bool loadingTop: false
    property bool recentDone: false
    property bool topDone: false
    property bool feedRotating: false
    // Активен ли Waiter первой загрузки. Нужен, чтобы скрыть его РОВНО ОДИН раз:
    // догрузка следующих страниц (loadMore) тоже шлёт recentFinished, и без этого
    // флага Waiter.hide() (анимация from:1.0) заново «моргал» бы.
    property bool waiterActive: false

    // Полная загрузка главной: показываем Waiter и грузим обе ленты. Waiter скрывается
    // только когда ОБЕ (top + recent) завершились — см. updateHomeWaiter.
    function startHomeLoad() {
        page.recentDone = false; page.topDone = false;
        page.loadingRecent = true; page.loadingTop = true;
        page.waiterActive = true;
        waiter.statusText = "Loading top rated and recent apps…";
        waiter.show();
        api.getRecentApps();
        api.getTopApps();
    }
    // Повтор загрузки из общего диалога ошибки (main.qml).
    function retry() { startHomeLoad(); }

    function updateHomeWaiter() {
        if (!page.waiterActive)
            return; // уже скрыли после первой партии — больше не трогаем
        if (page.recentDone && page.topDone) {
            page.waiterActive = false;
            waiter.hide();
        } else if (page.topDone) {
            waiter.statusText = "Loading recent apps…";
        } else if (page.recentDone) {
            waiter.statusText = "Loading top rated apps…";
        }
    }

    // Заголовок кликабелен — открывает меню (обновить ленту + тестовые страницы).
    property Component pageHeader: Component {
        Header {
            title: page.title
            clickable: true
            onClicked: homeMenu.open()
        }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground

    tools: toolBar

    // Первая загрузка; и повтор при возврате, если обе модели пусты (ничего не загрузилось).
    Component.onCompleted: startHomeLoad()
    onStatusChanged: {
        if (status === PageStatus.Activating
                && api.recentModel.count === 0 && api.topModel.count === 0
                && !page.loadingRecent && !page.loadingTop)
            startHomeLoad();
    }

    Menu {
        id: homeMenu
        MenuLayout {
            MenuItem { text: "Refresh"; onClicked: page.startHomeLoad() }
            MenuItem { text: "Queue test"; onClicked: appWindow.pageStack.push(testQueuePage) }
            MenuItem {
                text: "Installed test"
                // Без originPage: «назад» делает pop() на главную (homePage остаётся в
                // стеке). Передавать originPage:homePage нельзя — InstalledAppsPage делает
                // replace(originPage), а replace на страницу, что уже в стеке, бросает.
                onClicked: appWindow.pageStack.push(installedAppsPage, { testMode: true })
            }
        }
    }

    // Шапка ленты: заголовок + горизонтальная карусель «недавних» + заголовок списка.
    Component {
        id: feedHeader
        Column {
            width: mainList.width
            // Карусель «Top rated» — клиентское ранжирование api.topModel (см. getTopApps).
            // Скрываем секцию, пока топ не загрузился.
            SectionHeader { text: "Top rated" }
            ListView {
                id: carousel
                width: parent.width
                // ПОСТОЯННАЯ высота: иначе при поздней загрузке топа шапка списка
                // «вырастала» и ListView сдвигал contentY вниз (главная открывалась
                // на Recently updated). Фиксированная высота -> открывается с Top rated.
                height: 132
                orientation: ListView.Horizontal
                clip: true
                spacing: 8
                model: api.topModel
                delegate: Item {
                    width: 96
                    height: carousel.height
                    // Колонка прижата к верху -> все иконки на одном уровне независимо
                    // от длины названия (название — РОВНО одна строка с «…»).
                    Column {
                        anchors { top: parent.top; topMargin: 8; horizontalCenter: parent.horizontalCenter }
                        width: 88
                        spacing: 4
                        // При ошибке декодирования иконки — стандартная иконка магазина.
                        FallbackImage {
                            id: cIcon
                            width: 72; height: 72
                            anchors.horizontalCenter: parent.horizontalCenter
                            source: appIcon ? appIcon : ""
                            fallbackSource: "image://theme/icon-m-content-ovi-store-inverse"
                        }
                        Text {
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            text: appName
                            color: "black"
                            font.pixelSize: 15
                            elide: Text.ElideRight // одна строка, лишнее -> «…»
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        // Как на AppsPage: открываем через replace, «назад» -> replace(homePage).
                        onClicked: appWindow.pageStack.replace(appPage, { appId: appId, gradientSource: cIcon, replaceOrigin: homePage })
                    }
                }
            }
            SectionHeader { text: "Recently updated" }
        }
    }

    // Подвал: индикатор догрузки следующих страниц (свой LoadMoreRectangle).
    Component {
        id: feedFooter
        LoadMoreRectangle {
            id: footerItem
            width: mainList.width
            height: api.recentHasMore ? 90 : 0
            opacity: api.recentHasMore ? 1 : 0
            Behavior on height { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
            Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
            SectionHeader {}
            Connections {
                target: page
                onFeedRotatingChanged: page.feedRotating ? footerItem.startRotation() : footerItem.stopRotation()
            }
        }
    }

    ListView {
        id: mainList
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
        cacheBuffer: 1000
        model: api.recentModel
        header: feedHeader
        footer: feedFooter
        delegate: AppDelegate {
            onClicked: appWindow.pageStack.replace(appPage, { appId: appId, gradientSource: iconImage, replaceOrigin: homePage })
        }
        onAtYEndChanged: {
            if (atYEnd && api.recentHasMore) {
                page.feedRotating = true;
                api.loadMoreRecent();
            }
        }
    }

    // Пустое состояние (после неудачной/пустой загрузки — Waiter уже скрыт).
    Text {
        anchors.centerIn: parent
        text: "Couldn't load apps.\nTap the title → Refresh."
        horizontalAlignment: Text.AlignHCenter
        color: "#888888"
        font.pixelSize: 22
        visible: api.recentModel.count === 0 && !page.loadingRecent
    }

    Connections {
        target: api
        onRecentFinished: {
            page.loadingRecent = false;
            page.feedRotating = false;
            page.recentDone = true;
            page.updateHomeWaiter();
        }
        onTopFinished: {
            page.loadingTop = false;
            page.topDone = true;
            page.updateHomeWaiter();
        }
    }

    // Перекрывает страницу, пока не загрузятся ОБА списка (top + recent).
    Waiter { id: waiter }
}
