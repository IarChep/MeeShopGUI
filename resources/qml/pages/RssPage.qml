import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

import "../components/header"
import "../js/UIConstants.js" as UI
Page {
    id: page

    property string title : "<b>Meeshop:<b> Rss Feeds"
    property int count: 0

    //tools: commonTools
    orientationLock: PageOrientation.LockPortrait

    tools: toolBar

    // Передний план — стандартный (заголовок-текст). Фон — своя синяя плашка:
    // при заходе/уходе со страницы он растворяется (фон другой), а заголовок-текст
    // при этом меняется анимацией тулбара.
    property Component pageHeader: Component {
        Header { title: page.title }
    }
    property Component pageHeaderBackground: Component {
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#4DA1FF" }
                GradientStop { position: 1.0; color: "#1E90FF" }
            }
        }
    }
}
