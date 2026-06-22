import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0

// Элемент списка установленных приложений: иконка, название, разработчик и размер
// на диске, плюс две кнопки справа — удалить (запускает задачу удаления после
// подтверждения) и запустить (play). Похож на QueueDelegate, но без перетаскивания,
// без ExtendedIndicator; вместо статус-текста — размер.
Item {
    id: del

    property int itemHeight: 100
    // Можно ли запустить (есть .desktop). Считается один раз при создании делегата —
    // модель статична между refresh(), а делегаты пересоздаются при сбросе модели.
    property bool runnable: installedApps.isRunnable(index)

    width: ListView.view ? ListView.view.width : 480
    height: itemHeight

    Rectangle {
        id: content
        anchors.fill: parent
        color: "#e0e1e2"

        Item {
            id: iconBox
            width: 64
            height: 64
            anchors {
                left: parent.left
                leftMargin: 16
                verticalCenter: parent.verticalCenter
            }
            Image {
                id: iconImage
                anchors.centerIn: parent
                width: 64
                height: 64
                fillMode: Image.PreserveAspectFit
                source: model.iconSource ? model.iconSource
                                         : "image://theme/icon-m-content-ovi-store-inverse"
            }
        }

        Column {
            anchors {
                left: iconBox.right
                leftMargin: 16
                right: actions.left
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }
            spacing: 2

            Text {
                width: parent.width
                text: model.name
                color: "black"
                font.pixelSize: 24
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text: "By: " + model.developer
                color: "#666666"
                font.pixelSize: 18
                elide: Text.ElideRight
                visible: model.developer.length > 0
            }
            Text {
                width: parent.width
                text: model.sizeText
                color: "#888888"
                font.pixelSize: 18
                elide: Text.ElideRight
                visible: model.sizeText.length > 0
            }
        }

        // Кнопки справа: равные отступы сверху / между / снизу (как в QueueDelegate).
        Column {
            id: actions
            anchors {
                right: parent.right
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }
            spacing: (del.itemHeight - 2 * 40) / 3

            // Удалить приложение с устройства (после подтверждения — диалог на странице).
            Item {
                width: 40
                height: 40
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-delete"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: del.ListView.view.requestRemove(model.name, model.developer,
                                                               model.iconSource, model.packageName)
                }
            }

            // Запустить приложение. Неактивна, если у пакета нет .desktop.
            Item {
                width: 40
                height: 40
                opacity: del.runnable ? 1.0 : 0.3
                Image {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-toolbar-mediacontrol-play"
                }
                MouseArea {
                    anchors.fill: parent
                    enabled: del.runnable
                    onClicked: installedApps.launch(index)
                }
            }
        }

        // Разделитель между делегатами.
        Rectangle {
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            height: 1
            color: "#c6c7c8"
        }
    }
}
