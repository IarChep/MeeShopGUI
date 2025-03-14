import QtQuick 1.1
import com.nokia.meego 1.0
import IarChep.MeeShop 1.0
import "Pages"
import "Components"

SwipePageStackWindow {
    id: appWindow

    property int savedPage: 1

    initialPage: homePage

    Component {
        id: homePage
        HomePage {}
    }

    Component {
        id: appsPage
        AppsPage {}
    }
    Component {
        id: rssPage
        RssPage {}
    }
    Component {
        id: appPage
        AppPage {}
    }

    OpenReposApi {
        id: api

    }

    Gradienter {
        id: gradienter
    }

    CategoriesSheet {
        id: catSheet
    }

    PackageManager {
        id: packageManager
    }

}
