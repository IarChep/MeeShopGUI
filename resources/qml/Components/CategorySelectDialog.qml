import QtQuick 1.1
import com.nokia.meego 1.1
import "./UIConstants.js" as UI

SelectionDialog {
    id: root
    model: mainApi.categoryModel
    titleText: "Select a category:"

    property int selectedCatIndex: 0
    property int selectedSortIndex:0

    ListModel {
        id: sortbyModel
        ListElement{
            name: "By name"
            sortType: "title"
        }
        ListElement {
            name: "By author"
            sortType: "publisher"
        }
        ListElement {
            name: "By downloads"
            sortType: "downloads"
        }
    }


    SortByDelegate{id: sortbyDelegate}

    delegate: CategoryDelegate{id: categoryDelegate}

    buttons: Item {
        id: footer

        width: parent.width
        height: childrenRect.height

        Rectangle {
            id: sortByButton
            color: "transparent"
            height: 40
            anchors {
                right: parent.right
                left: parent.left
            }
            opacity: closeButtonArea.pressed ? 0.5 : 1.0
            Text {
                anchors.centerIn: parent
                text: "Sort By"
                color: "white"
                font.pixelSize: 32
            }

            MouseArea {
                id: closeButtonArea
                anchors.fill: parent
                onClicked: {
                    root.titleText = "Select a sort type:"
                    root.model = sortbyModel
                    root.delegate = sortbyDelegate
                }
            }

        }
    }

    onAccepted: {
        page.lastPage = 1
        mainApi.getCategoryContent(page.category, 0, page.sort)
    }
}
