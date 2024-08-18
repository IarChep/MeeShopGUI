// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.1
import IarChep.MeeShop 1.0


Item {
    id: pageSwitcher
    width: 314
    height: 165

    property int currentPage: 1
    property int totalPages
    property int shift: 0
    signal pageChanged()

    function setPage(page) {
        switch (currentPage < page) {
        case true:
            currentPage = page
            shift -= 180 * (Math.floor(currentPage / 4)) + shift
            if(currentPage > 2 && currentPage - Math.abs(shift)/45 < 3) {
                shift += 45 * (3 - (currentPage - Math.abs(shift)/45))
            } else if (currentPage - Math.abs(shift)/45 === 0) {
                shift -= 45
            }
            break
        case false:
            currentPage = page
            shift -= shift + (180 * (Math.floor(currentPage / 4)))
            if(currentPage > 2 && currentPage - Math.abs(shift)/45 > 2) {
                shift -= 45 * ((currentPage - Math.abs(shift)/45) - 2)
            } else if (currentPage - Math.abs(shift)/45 === 0) {
                shift += 90
            }
            break
        }
        pageChanged()
    }

    PageSelectionDialog {id: pageDialog}

    Column {
        spacing: 5
        Row {
            id: row
            spacing: 20

            Button {
                text: "←"
                enabled: pageSwitcher.currentPage > 1
                width: height
                onClicked: {
                    pageSwitcher.currentPage--;
                    if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 1 && pageSwitcher.currentPage != 1) {
                        pageSwitcher.shift += 45;
                    }
                }
            }

            Rectangle {
                id: clipRect
                width: 173
                height: 51
                color: "transparent"
                clip: true

                Row {
                    spacing: 20
                    x: 7 + pageSwitcher.shift
                    Behavior on x {
                        NumberAnimation {
                            duration: 100
                        }
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    Repeater {
                        model: pageSwitcher.totalPages

                        Item {
                            id: pageItem
                            width: 25
                            height: width
                            scale: pageSwitcher.currentPage === index + 1 ? 1.5 : 1

                            NokiaShape {
                                id: circle
                                anchors.centerIn: parent
                                width: parent.width
                                height: parent.height
                                borderVisible: true
                                borderColor: "black"
                                borderWidth: 2
                                color: pageSwitcher.currentPage === index + 1 ? "#1E90FF" : "white"

                                Text {
                                    anchors.centerIn: parent
                                    text: pageSwitcher.currentPage === index + 1 ? pageSwitcher.currentPage : ""
                                    color: "white"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        pageSwitcher.currentPage = index + 1
                                        if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 4 && pageSwitcher.currentPage != pageSwitcher.totalPages) {
                                            pageSwitcher.shift -= 45;
                                        } else if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 1 && pageSwitcher.currentPage != 1) {
                                            pageSwitcher.shift += 45;
                                        }
                                    }
                                }
                            }
                            Behavior on scale {NumberAnimation{ duration: 100; onRunningChanged: if (!running) {pageSwitcher.pageChanged()}}}
                        }
                    }
                }
            }

            Button {
                text: "→"
                width: height
                enabled: pageSwitcher.currentPage < pageSwitcher.totalPages
                onClicked: {
                    pageSwitcher.currentPage++;
                    if (pageSwitcher.currentPage - Math.abs(pageSwitcher.shift)/45 === 4 && pageSwitcher.currentPage != pageSwitcher.totalPages) {
                        pageSwitcher.shift -= 45;
                    }
                }
            }
        }
        Text {
            text: "Total pages: " + pageSwitcher.totalPages
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "Switch to page"
            onClicked: pageDialog.open()
        }
    }
    onCurrentPageChanged: pageDialog.selectedIndex = currentPage - 1
}
