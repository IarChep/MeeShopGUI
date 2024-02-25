import QtQuick 1.1
import com.nokia.meego 1.0

SwipePageStackWindow {
    id: appWindow

    initialPage: homePage

    HomePage {
        id: homePage
    }
    AppsPage {
        id: appsPage
    }
    RssPage {
        id: rssPage
    }
}
