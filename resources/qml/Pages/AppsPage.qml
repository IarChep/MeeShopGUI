import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title : "<strong>Meeshop: <strong>All Apps"
    property int count: 0

    tools: ToolBarLayout {
            id: commonTools
            visible: true
            ButtonRow {
                checkedButton: rightButton
                TabButton {
                    id: rightButton
                    text: "MeeShop's repo"
                }
                TabButton {
                    id: leftButton
                    text: "OpenRepos"
                }
            }
        }


    Header {id: header}

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
            count = 0
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white"});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white", highlited: true});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "./rss-page-icon.png"});
        }
    }
}
