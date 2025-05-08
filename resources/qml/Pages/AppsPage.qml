import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../Components"
import "../Components/UIConstants.js" as UI

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property int page: 0
    property string categoryName: "Apps"
    property string title : "<b>MeeShop</b>: " + categoryName
    property int category: 1
    property bool isSearch: false
    property string query: ""

       tools: toolBar

    Header {
        id: header
        clickable: true
        onClicked: {
            api.getCategories()
            catSheet.open()
        }
    }
    Rectangle {
        id: searchRect
        z: 1
        color: UI.COLOR_BACKGROUND
        anchors.top: header.bottom
        width: parent.width
        height: childrenRect.height + 10
        TextField {
            Image {
                source: "image://theme/icon-l-search-main-view"
                anchors.verticalCenter: parent.verticalCenter
                anchors {
                    left: parent.left
                    leftMargin: UI.MARGIN_XLARGE
                }
            }

            style: TextFieldStyle {
                paddingLeft:  72
            }
            anchors.centerIn: parent
            id: searchField
            width: parent.width - 20
            placeholderText: "Search " + page.categoryName
            onTextChanged: {
                if(searchField.text != "") {
                    page.isSearch = true
                    page.query = searchField.text;
                    mainApi.search(page.query, page.category, 0)
                    break;
                }
                page.isSearch = false;
                page.resetPages();
            }
        }
    }


    Component {
        id: headerItem
        LoadMoreRectangle {
            SectionHeader {
                anchors.bottom: parent.bottom
            }
        }
    }
    Component {
        id: footerItem
        LoadMoreRectangle {
            SectionHeader {}
        }
    }

    ListView {
        id: mainList
        width: parent.width
        anchors.top: searchRect.bottom
        anchors.topMargin: 5
        height:  parent.height - header.height - searchRect.height - anchors.topMargin
        model: api.appModel

        property bool endReached: false
        property bool startReached: true
        property int oldContentY;

        property bool showHeader: false
        property bool showFooter: true

        delegate: AppDelegate {
            onClicked: {
                appWindow.pageStack.replace(appPage, {appId: appId})
            }
        }

        header: mainList.showHeader ? headerItem : null
        footer: mainList.showFooter ? footerItem : null

        Connections {
            target: page
            onPageChanged: {
                console.log("Page: ", page.page)
                if (page.page > 2 && !mainList.showHeader) {
                    mainList.showHeader = true
                    mainList.oldContentY += 90
                }
                else if (page.page - 2 <= 0 && mainList.showHeader) {
                    mainList.showHeader = false
                    mainList.oldContentY -= 90
                }
            }
        }

        Connections {
            target: api.appModel
            onPageBackAdded: {
                mainList.endReached = false;
                mainList.startReached = false;
                mainList.contentY = mainList.oldContentY - (frontDeletedSize * 90)
            }
            onPageFrontAdded: {
                mainList.endReached = false;
                mainList.startReached = false;
                mainList.contentY = frontAddedSize * 90;
            }
        }


        onFlickEnded: {
            if (!endReached && mainList.contentY >= mainList.contentHeight - mainList.height) {
                endReached = true
                mainList.oldContentY = mainList.contentY
                console.log("end")
                page.page += 1
                api.getCategoryApps(page.category, page.page)
            } if (!startReached && mainList.contentY - 210<= 0 && page.page - 3 >= 0) {
                startReached = true
                mainList.oldContentY = mainList.contentY
                console.log("start")
                page.page -= 1
                api.getCategoryApps(page.category, page.page-2)
            }
        }
        Component.onCompleted: {
            endReached = false
            startReached = true
        }
    }


    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            api.getCategoryApps(page.category, page.page);
        }
    }
}
