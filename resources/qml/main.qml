import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "Pages"
import "Components"

SwipePageStackWindow {
    id: appWindow

    property int savedPage: 1

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

    OpenReposApi {
        id: api

    }

    Gradienter {
        id: gradienter
    }

    AppSheet {
        id: appSheet
    }
    CategoriesSheet {
        id: catSheet
    }

    PackageManager {
        id: packageManager
    }

}
