import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../components/header"
import "../components/views"
import "../js/UIConstants.js" as UI

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait
    property string title: "<b>MeeShop</b>: Queue"

    // Страница, с которой открыли очередь. Кнопка «назад» вернёт её через replace.
    // Типизированное (guarded) свойство: станет null, если страница исчезнет.
    property QtObject originPage: null

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
                checked: true               // уже здесь — переключать некуда
            }
            TabButton {
                text: "Installed"
                checked: false
                onClicked: appWindow.pageStack.replace(installedAppsPage,
                                                       { originPage: page.originPage })
                // Бейдж с количеством установленных приложений — прямо над текстом.
                CountBubble {
                    anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
                    value: installedApps.count
                    visible: value > 0
                }
            }
        }
    }

    // Содержимое единого заголовка для этой страницы + стандартный фон.
    property Component pageHeader: Component {
        Header { title: page.title }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground

    // Раздел очереди — единственный вход на страницу установленных, поэтому держим
    // их список свежим: бейдж на вкладке Installed и сами данные готовы к переходу.
    // (refresh() читает dpkg синхронно — приемлемо для небольшого числа приложений.)
    Component.onCompleted: installedApps.refresh()
    Connections {
        target: queue
        onTaskFinished: installedApps.refresh()
    }

    // Список очереди с перетаскиванием — от заголовка до toolbar.
    QueueListView {
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        // Тап по элементу — открыть страницу приложения (push, чтобы «назад»
        // вернул в очередь). Иконку отдаём градиенту, fromQueue — для кнопки назад.
        onAppActivated: appWindow.pageStack.push(appPage, { appId: appId, gradientSource: icon, fromQueue: true })
    }
}
