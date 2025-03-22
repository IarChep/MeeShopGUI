import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import IarChep.MeeShop 1.0

import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title: "<b>MeeShop</b>: Home"
    property int count: 0

    Header {id: header}



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
            count = 0
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white", highlited: true});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white"});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "../../images/rss-page-icon.png"});
        }
    }
}
