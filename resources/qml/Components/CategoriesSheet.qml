import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0

Sheet {

    title: Text {
        text: "Choose a category"
        font.pixelSize: 26
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter
        font.weight: Font.Light
    }

    buttons: Item {
        id: buttonRow
        anchors.fill: parent
        SheetButton {
            id: rejectButton
            objectName: "rejectButton"
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            text: "Cancel"
            visible: text != ""
            onClicked: close()
        }
        Component.onCompleted: {
            acceptButton.clicked.connect(accepted)
            rejectButton.clicked.connect(rejected)
        }
    }

    content: ListView {
        model: api.categoryModel
        anchors.fill:  parent
        delegate:
            CategoryDelegate{
            onClicked: {
                api.categoryModel.toggleKids(categoryName)
            }
        }
    }
}
