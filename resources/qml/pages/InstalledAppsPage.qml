import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import IarChep.MeeShop 1.0
import "../components/delegates"
import "../components/header"
import "../components/ui"
import "../js/UIConstants.js" as UI

// Страница приложений, установленных через MeeShop. Алфавитный список с секциями,
// штатным ScrollDecorator и быстрой навигацией по буквам (SectionScrollDecorator).
// Достижима из InstallationQueuePage по вкладкам (QueueTabsTools).
Page {
    id: page
    orientationLock: PageOrientation.LockPortrait
    property string title: "<b>MeeShop</b>: Installed"

    // Страница, с которой открыли раздел очереди/установленных (для кнопки «назад»).
    property QtObject originPage: null

    // Тестовый режим: наполнить список ФИКТИВНЫМИ приложениями (плейсхолдерами) для
    // проверки UI на симуляторе, где нет реально установленных через MeeShop приложений.
    // В этом режиме НЕ зовём refresh() (он перечитывает реальные данные и затёр бы
    // плейсхолдеры), а на выходе со страницы очищаем их.
    property bool testMode: false

    // Данные удаляемого приложения (для диалога подтверждения).
    property string removeName: ""
    property string removeDeveloper: ""
    property string removeIcon: ""
    property string removePackage: ""

    // Тулбар с вкладками (Queue / Installed). Логика переключения — как в main.qml:
    // переход на соседнюю вкладку через replace, активная вкладка — no-op. originPage
    // прокидываем дальше, чтобы кнопка «назад» вернула исходную страницу.
    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: {
                if (page.originPage)
                    appWindow.pageStack.replace(page.originPage)
                else
                    appWindow.pageStack.pop()
            }
        }
        ButtonRow {
            TabButton {
                text: "Queue"
                checked: false
                onClicked: appWindow.pageStack.replace(installationQueuePage,
                                                       { originPage: page.originPage })
            }
            TabButton {
                text: "Installed"
                checked: true               // уже здесь — переключать некуда
                // Бейдж с количеством установленных приложений — прямо над текстом.
                CountBubble {
                    anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
                    value: installedApps.count
                    visible: value > 0
                }
            }
        }
    }

    property Component pageHeader: Component {
        Header { title: page.title }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground

    // Перечитываем список при открытии и после завершения любой задачи очереди
    // (установка/удаление меняют список установленных через MeeShop). В тестовом
    // режиме вместо этого наполняем плейсхолдерами и не перечитываем.
    Component.onCompleted: {
        if (page.testMode)
            installedApps.addPlaceholders();
        else
            installedApps.refresh();
    }
    Component.onDestruction: {
        if (page.testMode)
            installedApps.clear();
    }
    Connections {
        target: queue
        onTaskFinished: if (!page.testMode) installedApps.refresh()
    }

    Item {
        id: listRegion
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Rectangle { anchors.fill: parent; color: UI.COLOR_BACKGROUND }

        Text {
            anchors.centerIn: parent
            text: "No apps installed via MeeShop"
            font.pixelSize: 22
            color: "#888888"
            visible: installedApps.count === 0
        }

        ListView {
            id: list
            anchors.fill: parent
            clip: true
            model: installedApps
            cacheBuffer: 2000
            delegate: InstalledAppDelegate {}

            section.property: "section"
            section.criteria: ViewSection.FullString
            section.delegate: SectionHeader {
                width: list.width
                text: section
            }

            // Запрос на удаление от делегата — сохраняем данные и спрашиваем подтверждение.
            function requestRemove(name, developer, icon, pkg) {
                page.removeName = name;
                page.removeDeveloper = developer;
                page.removeIcon = icon;
                page.removePackage = pkg;
                removeDialog.open();
            }
        }

        // Скролл-декоратор системного вида + быстрая навигация по буквам с пузырём
        // (нативный индикатор рисует сам компонент, отдельный ScrollDecorator не нужен).
        SectionScrollDecorator {
            anchors.fill: parent
            listView: list
        }
    }

    // Подтверждение удаления: запускаем задачу удаления в общей очереди.
    QueryDialog {
        id: removeDialog
        titleText: "Remove application"
        message: "Remove " + page.removeName + " from your device?"
        acceptButtonText: "Remove"
        rejectButtonText: "Cancel"
        onAccepted: queue.enqueueRemovalByPackage(page.removeName, page.removeDeveloper,
                                                  page.removeIcon, page.removePackage)
    }
}
