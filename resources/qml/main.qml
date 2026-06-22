import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import IarChep.MeeShop 1.0
import "pages"
import "components/header"

PageStackWindow {
    id: appWindow

    property int savedPage: 1
    property int openedPage: 0

    initialPage: homePage

    // --- Единый заголовок ---
    // Стандартный фон заголовка — ОДИН общий Component: одинаковая ссылка у всех
    // обычных страниц => при переходах между ними фон не перерисовывается (анимация
    // фона играет только когда он реально другой, напр. синяя плашка RssPage).
    property Component stdHeaderBackground: Component {
        Image {
            anchors.fill: parent
            source: "../assets/images/headerImage.png"
        }
    }

    // Тип перехода (push/pop/replace/set) выводим из изменения глубины стека,
    // чтобы заголовок анимировался в такт со сменой страниц и тулбаром.
    property int __prevDepth: 0
    function __updateHeader() {
        var cp = pageStack.currentPage;
        var content = (cp && cp.pageHeader) ? cp.pageHeader : null;
        var bg = (cp && cp.pageHeaderBackground) ? cp.pageHeaderBackground : null;
        var d = pageStack.depth;
        var t = (__prevDepth === 0) ? "set"
              : (d > __prevDepth)    ? "push"
              : (d < __prevDepth)    ? "pop"
              :                        "replace";
        __prevDepth = d;
        headerBar.setHeader(content, bg, t);
    }
    Connections {
        target: pageStack
        onCurrentPageChanged: appWindow.__updateHeader()
    }

    ToolBarLayout {
        id: toolBar
        visible: false
        ButtonRow {
            TabButton {
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-home"
                }
                checked: openedPage == 0
                onClicked: {
                    if (openedPage == 1) {
                        appWindow.pageStack.pop()
                    } if (openedPage == 2) {
                        appWindow.pageStack.pop(homePage)
                    }

                    openedPage = 0
                }
            }
            TabButton {
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-application"
                }
                checked: openedPage == 1
                onClicked: {
                    if (openedPage == 0) {
                        appWindow.pageStack.push(appsPage)
                    } else if (openedPage == 2) {
                        appWindow.pageStack.pop()
                    }

                    openedPage = 1
                }
            }
            TabButton {
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-grid"
                }
                checked: openedPage == 2
                onClicked: {
                    if (openedPage == 0) {
                        appWindow.pageStack.push([appsPage, rssPage])
                    } else if (openedPage == 1) {
                        appWindow.pageStack.push(rssPage)
                    }
                    openedPage = 2
                }
            }
        }
        ToolIcon {
            id: queueButton
            iconSource: "image://theme/icon-s-transfer-download"
            // Заменяем текущую страницу очередью, запоминая, откуда пришли,
            // чтобы кнопка «назад» вернула replace'ом ту же страницу.
            onClicked: appWindow.pageStack.replace(installationQueuePage,
                                                   { originPage: appWindow.pageStack.currentPage })
            // Голубой бейдж: количество активных и ожидающих задач (без завершённых).
            CountBubble {
                anchors {
                    top: parent.top
                    right: parent.right
                    topMargin: 2
                    rightMargin: 6
                }
                value: queue.activeCount
                visible: value > 0
            }
        }
    }

    // Постоянные экземпляры (а не Component): переживают pageStack.replace,
    // поэтому к ним можно вернуться по id из InstallationQueuePage.
    HomePage {
        id: homePage
    }
    AppsPage {
        id: appsPage
    }
    RssPage {
        id: rssPage
    }
    Component {
        id: appPage
        AppPage {}
    }
    Component {
        id: developerPage
        DeveloperPage {}
    }
    Component {
        id: installationQueuePage
        InstallationQueuePage {}
    }
    Component {
        id: installedAppsPage
        InstalledAppsPage {}
    }
    Component {
        id: testQueuePage
        TestQueuePage {}
    }

    // Общий диалог сетевой ошибки: показывается при ЛЮБОЙ неудачной загрузке
    // (главная, страница приложения, список, разработчик и т.д.) — api.networkError.
    QueryDialog {
        id: networkErrorDialog
        titleText: "Connection failed!"
        message: "Couldn't reach the server.\nPlease check your internet connection.\n\nIf that doesn't help, the server may be under maintenance."
        acceptButtonText: "Retry"
        rejectButtonText: "Cancel"
        // Новая ошибка, прилетевшая пока диалог ещё закрывается (после Retry) — покажем
        // его снова, как только он полностью закроется. Иначе повторный сбой терялся
        // (guard status===Closed не срабатывал во время Closing) и диалог не возвращался.
        property bool pendingError: false
        // Retry перезагружает ту страницу, на которой случился сбой (у неё есть retry()).
        onAccepted: {
            var p = appWindow.pageStack.currentPage;
            if (p && p.retry)
                p.retry();
        }
        onStatusChanged: {
            if (status === DialogStatus.Closed && pendingError) {
                pendingError = false;
                open();
            }
        }
        // Cancel — просто закрыть (диалог закрывается сам).
    }
    Connections {
        target: api
        onNetworkError: {
            // Часто прилетает несколько ошибок сразу / сразу после Retry:
            //  - закрыт  -> показываем;
            //  - закрывается -> запомним и покажем снова после закрытия;
            //  - открыт/открывается -> уже показан, ничего не делаем.
            if (networkErrorDialog.status === DialogStatus.Closed)
                networkErrorDialog.open();
            else if (networkErrorDialog.status === DialogStatus.Closing)
                networkErrorDialog.pendingError = true;
        }
    }

    // Единый заголовок. Лежит ВНУТРИ области страниц (под скруглёнными углами и под
    // слоем Sheet/Menu/Dialog, которые крепятся к appWindowContent), но поверх самих
    // страниц. Не входит в стек страниц, поэтому не «уезжает» вместе с ними —
    // меняется только его содержимое (с анимациями тулбара) и фон (растворением).
    HeaderBar {
        id: headerBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        // Перемещаем из корня окна в область страниц (родитель PageStack), чтобы
        // скруглённые углы и всплывающие Sheet/Menu/Dialog корректно его перекрывали.
        Component.onCompleted: parent = pageStack.parent
    }
}
