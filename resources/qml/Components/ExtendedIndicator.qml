import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0

Item {
    id: root
    property string type: "busy" // "indicator" or "progress"
    property bool running: false
    property int progress: 0
    property string size: "medium"
    implicitWidth: root.size == "tiny" ? 24 : root.size == "small" ? 32 : root.size == "medium" ? 64 : 96;
    implicitHeight: implicitWidth
    // Выбираем элемент в зависимости от значения type
    Loader {
        id: loader
        anchors.centerIn: parent
        sourceComponent: root.type === "busy" ? busyIndicatorComponent : progressIndicatorComponent
    }

    // Компонент BusyIndicator
    Component {
        id: busyIndicatorComponent
        BusyIndicator {
            running: root.running
            implicitWidth: root.implicitWidth
        }
    }

    // Компонент CircleProgress
    Component {
        id: progressIndicatorComponent
        ProgressIndicator {
            size: root.size
            progress: root.progress
        }
    }
}
