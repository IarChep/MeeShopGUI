import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title: "<b>Meeshop:<b> Home"
    property int count: 0

    Header {id: header}

    ListView {
        id: mainList
        width: parent.width
        anchors.top: header.bottom
        height:  parent.height - header.height

        delegate: AppDelegate {
            onClicked: {
                appSheet.argList = [app_name, app_ver, app_size,app_pkg_name, "http://wunderwungiel.pl/MeeGo/openrepos/icons/" + app_icon]
                appSheet.open()
            }
        }

        section.property: "letter"
        section.criteria: ViewSection.FullString
        section.delegate: SectionDelegate {}
    }

    //Connection to avoid ListView bugs
    Connections {
        target: mainRepo

        onInformation_loading_finished: {
            mainRepo.model.slice(0,9)
            mainList.model = mainRepo.model
        }
    }


    // do not touch, SwipePageStackWindow connections!
    Connections{
        target: page.status === PageStatus.Active ? appWindow : null
        ignoreUnknownSignals: true
        onClicked:{
            if(type === "apps")
                appWindow.pageStack.push(appsPage);
            else if(type === "rss")
                appWindow.pageStack.push([appsPage,rssPage]);
        }
        onLeftSwipe: {
            if(count === 0) {
                appWindow.pageStack.push(appsPage)
                count = 1
            }
        }
    }

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            mainRepo.load_applications("catalog_new.xml")
            count = 0
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white", highlited: true});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white"});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "rss-page-icon.png"});
        }
    }
}
