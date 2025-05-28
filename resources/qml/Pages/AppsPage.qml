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
    property int count: 0
    property int category: 1
    property bool isSearch: false
    property string query: ""

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


<<<<<<< HEAD

=======
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
            height: (mainList.showFooter && api.isNextPageAvailible) ? 90 : 0
            opacity: (mainList.showFooter && api.isNextPageAvailible) ? 1 : 0
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
    Rectangle {
        color: "transparent"
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        Text {
            anchors.centerIn: parent
            id: emptyCatText
            text: "This category is empty :("
            font.pixelSize: 20
            visible: api.appModel.isEmpty()
        }
    }



>>>>>>> 982f1f7 (Almost finished app list functionality. Categories now fully work)
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

        delegate: AppDelegate {
            onClicked: {
                appWindow.pageStack.replace(appPage, {appId: appId})
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

<<<<<<< HEAD

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
=======
        onAtYEndChanged: {
            if (atYEnd && !endReached && api.isNextPageAvailible) {
                endReached = true;
                page.footerRotating = true;
                console.log("end");
                page.page += 1;
                api.getCategoryAppsPage(page.category, page.page + 1);
>>>>>>> 982f1f7 (Almost finished app list functionality. Categories now fully work)
            }
        }
        Component.onCompleted: {
            endReached = false
            startReached = true
        }
    }

<<<<<<< HEAD
    Connections{
        target: page.status === PageStatus.Active ? appWindow : null
        ignoreUnknownSignals: true
        onClicked:{
            if(type === "home")
                appWindow.pageStack.pop(homePage);
            else if(type === "rss")
                appWindow.pageStack.push(rssPage);
        }
        onLeftSwipe: {
            if (count === 0) {
                appWindow.pageStack.push(rssPage);
                count = 1
=======
    Waiter {
        id: waiter
        Connections {
            target: api
            onFinished: {
                if (waiter.opacity == 1.0) {
                    waiter.hide()
                }
>>>>>>> 982f1f7 (Almost finished app list functionality. Categories now fully work)
            }
        }
        onRightSwipe: {
            appWindow.pageStack.pop(homePage);
        }
    }

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
<<<<<<< HEAD
            count = 0;
            api.getCategoryApps(page.category, page.page);
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white"});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white", highlited: true});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "../../images/rss-page-icon.png"});
=======
            if (page.category === -1) {
                catSheet.cancellable = false
                api.getCategories()
                catSheet.open()
            }
        }
    }
    Connections {
        target: catSheet
        onAccepted: {
            page.page = 0;
            waiter.show();
            mainList.resetVars();
            api.getCategoryApps(page.category);
>>>>>>> 982f1f7 (Almost finished app list functionality. Categories now fully work)
        }
    }
}
