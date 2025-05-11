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

    property bool footerRotating: false;
    property bool headerRotating: false;

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
        id: listHeader
        LoadMoreRectangle {
            id: headerItem
            height: mainList.showHeader ? 90 : 0
            opacity: mainList.showHeader ? 1 : 0
            Behavior on height {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            SectionHeader {
                anchors.bottom: parent.bottom
            }
            Connections {
                target: page
                onHeaderRotatingChanged: {
                    if (page.headerRotating) {
                        headerItem.startRotation()
                    } else {
                        headerItem.stopRotation()
                    }
                }
            }
        }
    }
    Component {
        id: listFooter
        LoadMoreRectangle {
            id: footerItem
            height: mainList.showFooter ? 90 : 0
            opacity: mainList.showFooter ? 1 : 0
            Behavior on height {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }
            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }

            SectionHeader {}
            Connections {
                target: page
                onFooterRotatingChanged: {
                    if (page.footerRotating) {
                        footerItem.startRotation()
                    } else {
                        footerItem.stopRotation()
                    }
                }
            }
        }
    }

    ListView {
        id: mainList
        width: parent.width
        anchors.top: searchRect.bottom
        anchors.topMargin: 5
        height:  parent.height - header.height - searchRect.height - anchors.topMargin
        model: api.appModel

        property bool endReached: true
        property bool startReached: true

        property bool showHeader: false
        property bool showFooter: true

        delegate: AppDelegate {
            onClicked: {
                appWindow.pageStack.replace(appPage, {appId: appId})
            }
        }

        header: listHeader
        footer: listFooter

        Connections {
            target: page
            onPageChanged: {

                console.log("Page: ", page.page)
                if (page.page > 2 && !mainList.showHeader) {
                    mainList.showHeader = true
                }
                else if (page.page - 2 <= 0 && mainList.showHeader) {
                    mainList.showHeader = false
                }
            }
        }

        Connections {
            target: api.appModel
            onPageBackAdded: {
                mainList.endReached = false;
                mainList.startReached = false;
                page.footerRotating = false;
            }
            onPageFrontAdded: {
                mainList.endReached = false;
                mainList.startReached = false;
                mainList.contentY += frontAddedSize * 90;
                page.headerRotating = false;
            }
        }

        onAtYEndChanged: {
            if (atYEnd && !endReached) {
                endReached = true;
                page.footerRotating = true;
                console.log("end");
                page.page += 1;
                api.getCategoryApps(page.category, page.page);

            }
        }
        onAtYBeginningChanged: {
            if (atYBeginning && !startReached && page.page - 3 >= 0) {
                startReached = true;
                page.headerRotating = true;
                console.log("start");
                page.page -= 1;
                api.getCategoryApps(page.category, page.page-2);

            }
        }
    }


    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            api.getCategoryApps(page.category, page.page);
        }
    }
}
