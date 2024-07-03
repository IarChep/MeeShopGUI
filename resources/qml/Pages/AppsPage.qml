import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property int lastPage: 0
    property string title : "<b>Meeshop</b>: Apps"
    property int count: 0
    property int category: 1
    property string sort: "title"

    CategorySelectDialog {id: categoryDialog}

    Header {
        id: header
        clickable: true
        onClicked: {
            mainApi.getCategories()
            categoryDialog.open()
        }
    }

    ListView {
        id: mainList
        width: parent.width
        anchors.top: header.bottom
        height:  parent.height - header.height
        model: mainApi.appModel

        delegate: AppDelegate {
            higlightLetters: true
            onClicked: {
                appSheet.argList = [appName, appVer, appSize,appPkgName, "http://wunderwungiel.pl/MeeGo/openrepos/icons/" + appIcon]
                appSheet.open()
            }
        }

        section.property: page.sort === "publisher" ? "devLetter" : "letter"
        section.criteria: ViewSection.FullString
        section.delegate: SectionDelegate {}
        footer: PageSwitcher {
            id: switcher
            totalPages: mainApi.getPages(category)
            anchors.horizontalCenter: parent.horizontalCenter
            onPageChanged: {page.lastPage = currentPage-1; mainApi.getCategoryContent(page.category, currentPage-1, page.sort)}
            Component.onCompleted: pagesModel.setPages(totalPages)
            Connections {
                target: categoryDialog
                onAccepted: {
                    switcher.currentPage = 1
                    pagesModel.setPages(switcher.totalPages)

                }
            }
        }
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
            mainApi.getCategoryContent(page.category, page.lastPage, page.sort)
            count = 0
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white"});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white", highlited: true});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "./rss-page-icon.png"});
        }
    }
}
