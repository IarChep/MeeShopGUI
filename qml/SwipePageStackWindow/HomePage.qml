import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

Page {
    id: page

    property string title : "<b>Meeshop:<b> Home"

    //tools: commonTools
    orientationLock: PageOrientation.LockPortrait
    Image {
        id: pageHeader
        anchors {
            top: page.top
            left: page.left
            right: page.right
        }

        height: parent.width < parent.height ? 72 : 46
        width: parent.width
        source: "image://theme/meegotouch-view-header-fixed" + (theme.inverted ? "-inverted" : "")
        z: 1

        Label {
            id: header
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 16
            }
            platformStyle: LabelStyle {
                fontFamily: "Nokia Pure Text Light"
                fontPixelSize: 32
            }
            text: page.title
        }
    }

    ListView {
        id: listView
        anchors {
            top: pageHeader.bottom
            bottom: page.bottom
            left: page.left
            right: page.right
            margins: 16
        }
        model: ListModel{id: listModel}
        delegate: Rectangle{
            id: listDelegate
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    appWindow.pageStack.push(Qt.resolvedUrl("Test.qml"), {title: title});
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    Connections{
        target: page.status === PageStatus.Active ? appWindow : null // Por si se agrega otro Item al pageStack
        ignoreUnknownSignals: true
        onClicked:{
            if(type === "apps")
                appWindow.pageStack.push(appsPage);
            else if(type === "rss")
                appWindow.pageStack.push([appsPage,rssPage]);
        }
        onLeftSwipe: {
            console.log("left swipe")
            appWindow.pageStack.push(appsPage);
        }
    }

    onStatusChanged: {
        if(status === PageStatus.Activating)
        {
            appWindow.menuModel.clear();
            appWindow.menuModel.append({title: "Home", type:"home", iconSource: "image://theme/icon-m-toolbar-home-white", highlited: true});
            appWindow.menuModel.append({title: "Apps", type:"apps", iconSource: "image://theme/icon-m-toolbar-application-white"});
            appWindow.menuModel.append({title: "RSS Feeds", type:"rss", iconSource: "./rss-page-icon.png"});
            appWindow.menuModel.append({title: "About", type:"about", iconSource: "image://theme/icon-m-toolbar-view-menu-white"});
        }
    }
}
