import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "Pages"
import "Components"

PageStackWindow {
    id: appWindow

    property int savedPage: 1
    property int openedPage: 0

    initialPage: homePage


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
            platformIconId: "toolbar-view-menu"
        }
    }

    Component {
        id: homePage
        HomePage {}
    }

    Component {
        id: appsPage
        AppsPage {}
    }
    Component {
        id: rssPage
        RssPage {}
    }
    Component {
        id: appPage
        AppPage {}
    }

    CategoriesSheet {
        id: catSheet
    }
}
