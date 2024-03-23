import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "Pages"
import "Components"

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


    MeeShopRepository {
        id: mainRepo
    }
    Gradienter {
        id: gradienter
    }

    AppSheet {
        id: appSheet
    }
}
