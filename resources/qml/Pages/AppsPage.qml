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
    property string title : "<b>Meeshop</b>: " + categoryName
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
        SearchField {
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
            higlightLetters: true
            onClicked: {
                appSheet.appId = appId
                appSheet.open()
            }
        }
        Connections {
            target: api.appModel
            onPageBackAdded: {
                mainList.endReached = false
                mainList.startReached = false
                mainList.contentY = mainList.oldContentY - (frontDeletedSize * 90)
            }
        }


        onFlickEnded: {
            if (!endReached && mainList.contentY >= mainList.contentHeight - mainList.height) {
                console.log("end")
                mainList.oldContentY = mainList.contentY
                page.page += 1
                api.getCategoryApps(page.category, page.page)
                endReached = true
            } if (!startReached && mainList.contentY <= 0 && page.page - 3 > 0) {
                console.log("start")
                //page.page -= 1
                //api.getCategoryApps(page.category, page.page)
                startReached = true
            }
        }
        Component.onCompleted: {
            endReached = false
            startReached = true
        }

//        footer: PageSwitcher {
//            id: switcher
//            totalPages: {
//                if(!page.isSearch) {
//                                mainApi.getPages(category)
//                }
//                mainApi.getSearchPages(page.query, page.category)
//            }
//            anchors.horizontalCenter: parent.horizontalCenter
//            onPageChanged: {
//                page.lastPage = currentPage-1
//                if(!isSearch)
//                {
//                    mainApi.getCategoryContent(page.category, currentPage-1, page.sort)
//                    break
//                }
//                mainApi.getSearchPages(page.query, page.category)
//            }
//            Component.onCompleted: pagesModel.setPages(totalPages)
//            Connections {
//                target: categoryDialog
//                onAccepted: {
//                    switcher.currentPage = 1
//                    pagesModel.setPages(switcher.totalPages)
//                }
//            }
//            Connections {
//                target: page
//                onResetPages: switcher.currentPage = 1
//            }
//        }
    }

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
            }
        }
        onRightSwipe: {
            appWindow.pageStack.pop(homePage);
        }
    }

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            count = 0;
            api.getCategoryApps(page.category, page.page);
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white"});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white", highlited: true});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "../../images/rss-page-icon.png"});
        }
    }
}
