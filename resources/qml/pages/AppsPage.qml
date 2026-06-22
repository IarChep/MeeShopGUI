import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../components/delegates"
import "../components/header"
import "../components/ui"
import "../components/sheets"
import "../js/UIConstants.js" as UI

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string categoryName: "Apps"
    property string title : "<b>MeeShop</b>: " + categoryName
    property int category: -1
    property bool isSearch: false
    property string query: ""

    property bool footerRotating: false;
    property bool headerRotating: false;

    // Повтор загрузки из общего диалога ошибки (main.qml): повторяем текущий запрос.
    function retry() {
        if (page.isSearch && page.query !== "") {
            waiter.show();
            api.search(page.query);
        } else if (page.category >= 0) {
            waiter.show();
            api.getCategoryApps(page.category);
        }
    }

    tools: toolBar

    CategoriesSheet {
        id: catSheet
    }

    // Содержимое единого заголовка: кликабельно (открывает выбор категорий) —
    // активно только пока эта страница текущая. Фон — стандартный.
    property Component pageHeader: Component {
        Header {
            title: page.title
            clickable: true
            onClicked: {
                catSheet.selectedIndex = page.category
                catSheet.cancellable = true
                catSheet.open()
            }
        }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground
    Rectangle {
        id: searchRect
        z: 1
        color: UI.COLOR_BACKGROUND
        anchors.top: parent.top
        anchors.topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
        width: parent.width
        height: childrenRect.height + 10
        TextField {
            Image {
                source: "image://theme/icon-l-search-main-view"
                anchors.verticalCenter: parent.verticalCenter
                anchors {
                    left: parent.left
                    leftMargin: UI.MARGIN_XLARGE
                }
            }

            style: TextFieldStyle {
                paddingLeft:  72
            }
            anchors {
                top: parent.top
                topMargin: 5
                horizontalCenter: parent.horizontalCenter
            }
            id: searchField
            width: parent.width - 20
            placeholderText: "Search " + page.categoryName
            onTextChanged: {
                page.query = searchField.text;
                searchTimer.restart();
            }
        }
    }


    // Дебаунс поиска: запрос уходит через паузу после последнего ввода.
    Timer {
        id: searchTimer
        interval: 350
        onTriggered: {
            page.isSearch = (page.query !== "");
            if (page.isSearch) {
                waiter.show();
                api.search(page.query);
            } else if (page.category >= 0) {
                waiter.show();
                api.getCategoryApps(page.category);
            }
        }
    }

    Component {
        id: listHeader
        LoadMoreRectangle {
            id: headerItem
            height: api.isPrevPageAvailable ? 90 : 0
            opacity: api.isPrevPageAvailable ? 1 : 0
            Behavior on height {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            SectionHeader {
                anchors.bottom: parent.bottom
            }
            Connections {
                target: page
                onHeaderRotatingChanged: {
                    if (page.headerRotating) {
                        headerItem.startRotation()
                    } else {
                        headerItem.stopRotation()
                    }
                }
            }
        }
    }
    Component {
        id: listFooter
        LoadMoreRectangle {
            id: footerItem
            height: api.isNextPageAvailable ? 90 : 0
            opacity: api.isNextPageAvailable ? 1 : 0
            Behavior on height {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }

            SectionHeader {}
            Connections {
                target: page
                onFooterRotatingChanged: {
                    if (page.footerRotating) {
                        footerItem.startRotation()
                    } else {
                        footerItem.stopRotation()
                    }
                }
            }
        }
    }
    Rectangle {
        color: "transparent"
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        Text {
            anchors.centerIn: parent
            id: emptyCatText
            text: "This category is empty :("
            font.pixelSize: 20
            visible: mainList.count === 0
        }
    }



    ListView {
        id: mainList
        width: parent.width
        anchors.top: searchRect.bottom
        anchors.topMargin: 5
        height: parent.height - UI.HEADER_DEFAULT_HEIGHT_PORTRAIT - searchRect.height - anchors.topMargin
        model: api.appModel

        delegate: AppDelegate {
            onClicked: {
                // Передаём уже загруженную иконку делегата на AppPage —
                // её Gradienter снимет цвета с неё сам, без повторной загрузки.
                appWindow.pageStack.replace(appPage, {appId: appId, gradientSource: iconImage})
            }
        }

        header: listHeader
        footer: listFooter

        Connections {
            target: api.appModel
            // При добавлении страницы спереди сдвигаем вьюпорт, чтобы
            // позиция прокрутки осталась на тех же элементах.
            onPageFrontAdded: mainList.contentY += frontAddedSize * 90;
        }

        onAtYEndChanged: {
            if (atYEnd && api.isNextPageAvailable) {
                page.footerRotating = true;
                api.loadNextPage();
            }
        }
        onAtYBeginningChanged: {
            if (atYBeginning && api.isPrevPageAvailable) {
                page.headerRotating = true;
                api.loadPrevPage();
            }
        }
    }

    Waiter {
        id: waiter
        Connections {
            target: api
            onFinished: {
                page.footerRotating = false;
                page.headerRotating = false;
                if (waiter.opacity == 1.0) {
                    waiter.hide()
                }
            }
        }
    }
    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            if (page.category === -1) {
                catSheet.cancellable = false
                api.getCategories()
                catSheet.open()
            }
        }
    }
    Connections {
        target: catSheet
        onAccepted: {
            waiter.show();
            api.getCategoryApps(page.category);
        }
    }
}
