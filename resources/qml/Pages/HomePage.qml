import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import IarChep.MeeShop 1.0

import "../Components"

Page {
    id: page
    orientationLock: PageOrientation.LockPortrait

    property string title: "<b>MeeShop</b>: Home"

    Header {id: header}

    ButtonRow {
        anchors.centerIn: parent
        width: 400
        exclusive: false
        __maxButtonSize: 400;

        Button {
            text: "test"
            visible: true
            onClicked: {
                Qt.openUrlExternally("http://openrepos.wunderwungiel.pl/sites/default/files/packages/16417/icon-icon-123.png")
            }
        }
    }

    tools: toolBar
}
