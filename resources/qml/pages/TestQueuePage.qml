import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "../components/header"
import "../components/views"
import "../js/UIConstants.js" as UI

// Тестовая страница очереди: позволяет наполнить список ФИКТИВНЫМИ записями
// (плейсхолдерами) разных статусов и проверять перетаскивание / удаление /
// просмотр лога БЕЗ установки реальных приложений. Использует тот же QueueListView
// и тот же контекст-объект `queue`, что и боевая страница, поэтому проверяется
// настоящая логика. Заполнять лучше при пустой очереди.
Page {
    id: page
    orientationLock: PageOrientation.LockPortrait
    property string title: "<b>MeeShop</b>: Queue test"

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop()
        }
        ToolIcon {
            platformIconId: "toolbar-add"
            onClicked: addMenu.open()
        }
        ToolIcon {
            platformIconId: "toolbar-delete"
            onClicked: queue.clearPlaceholders()
        }
    }

    // Уходя со страницы, убираем плейсхолдеры, чтобы они не попали в реальную
    // очередь (она общая) и не мешали настоящим установкам.
    Component.onDestruction: queue.clearPlaceholders()

    // Единый заголовок: стандартный вид.
    property Component pageHeader: Component {
        Header { title: page.title }
    }
    property Component pageHeaderBackground: appWindow.stdHeaderBackground

    function addSampleSet() {
        // Типичный набор: завершённое, выполняемое и несколько ожидающих —
        // удобно сразу проверить границу перетаскивания и индикаторы.
        queue.addPlaceholder(InstallationQueue.Done);
        queue.addPlaceholder(InstallationQueue.Running);
        queue.addPlaceholder(InstallationQueue.Queued);
        queue.addPlaceholder(InstallationQueue.Queued);
        queue.addPlaceholder(InstallationQueue.Queued);
    }

    // Меню добавления плейсхолдеров.
    Menu {
        id: addMenu
        MenuLayout {
            MenuItem { text: "Add sample set";   onClicked: page.addSampleSet() }
            MenuItem { text: "Add queued";        onClicked: queue.addPlaceholder(InstallationQueue.Queued) }
            MenuItem { text: "Add running";       onClicked: queue.addPlaceholder(InstallationQueue.Running) }
            MenuItem { text: "Add done";          onClicked: queue.addPlaceholder(InstallationQueue.Done) }
            MenuItem { text: "Add failed";        onClicked: queue.addPlaceholder(InstallationQueue.Failed) }
        }
    }

    // Тот же список с перетаскиванием, что и на боевой странице.
    QueueListView {
        anchors {
            top: parent.top
            topMargin: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        // Тап по элементу — открыть страницу приложения (push; «назад» вернёт в
        // очередь). Плейсхолдеры с appId<0 тап не открывают (см. QueueDelegate).
        onAppActivated: appWindow.pageStack.push(appPage, { appId: appId, gradientSource: icon, fromQueue: true })
    }
}
