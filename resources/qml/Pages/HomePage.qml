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
    tools: toolBar
}
