import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title: "<b>Meeshop</b>: Home"
    property int count: 0

    Header {id: header}

    Rectangle {
        id: textHeaderRect
        y: 300 + header.height
        color: "transparent"
        height: 150
        width: parent.width
        Text {
            font.pixelSize: 20
            text: "I am the text"
            anchors.centerIn: parent
        }
        z: 1
    }

    Flickable {
        id: flickable
        property int oldContentY: 0
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: header.bottom
        }
        contentHeight: rec1.height + rec2.height + rec3.height
        boundsBehavior: Flickable.StopAtBounds // Отключение эффекта "отпрыгивания"

        Rectangle {
            id: rec1
            color: "yellow"
            height: 300
            width: parent.width
        }

        Rectangle {
            id: rec2
            anchors.top: rec1.bottom
            color: "transparent"
            height: 150
            width: parent.width
        }

        Rectangle {
            id: rec3
            anchors.top: rec2.bottom
            color: "red"
            height: 800
            width: parent.width
        }

        onContentYChanged: {
            if (contentY < 300) {
                textHeaderRect.y-= (contentY - oldContentY)
                textHeaderRect.y = Math.min(300 + header.height, textHeaderRect.y)
            }

            if (contentY > rec1.height) {
                textHeaderRect.y = header.height
            } else {
                textHeaderRect.y = Math.min(300 + header.height, textHeaderRect.y)
            }

            oldContentY = contentY
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
            count = 0
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white", highlited: true});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white"});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "../../images/rss-page-icon.png"});
        }
    }
}
