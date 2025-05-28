import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import "../Components/UIConstants.js" as UI
Page {
    id: page

    property string title : "<b>Meeshop:<b> Rss Feeds"
    property int count: 0

    //tools: commonTools
    orientationLock: PageOrientation.LockPortrait

    tools: toolBar

    Rectangle{
             id: header
             anchors {
                 top: page.top
                 left: page.left
                 right: page.right
             }
             width: parent.width
             height: UI.HEADER_DEFAULT_HEIGHT_PORTRAIT
             gradient: Gradient {
                 GradientStop { position: 0.0; color: "#4DA1FF" }
                 GradientStop { position: 1.0; color: "#1E90FF" }
             }
             Text {
                 text: title
                 font.pixelSize: 32
                 color: "white"
                 anchors.left: parent.left
                 anchors.leftMargin: 20
                 verticalAlignment: Text.AlignVCenter
                 height: parent.height
                 z:0
             }
        }
}
