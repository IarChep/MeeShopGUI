#include <QApplication>
#include <QScopedPointer>

#include "qmlapplicationviewer/qmlapplicationviewer.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeView>

#include "tools/swipecontrol.h"

#include "tools/openreposapi.h"
#include "models/MeeShopApplicationModel.h"
#include "models/MeeShopCategoriesModel.h"
#include "qml_elements/Gradienter.h"
#include "tools/PackageManager.h"
#include "qml_elements/nokiashape.h"
#include "qml_elements/ProgressIndicator.h"
#include "structs/applicationinfo.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<MeeShop::OpenReposApi>("IarChep.MeeShop", 1, 0, "OpenReposApi");
    qmlRegisterType<MeeShop::MeeShopApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel");
    qmlRegisterType<MeeShop::MeeShopCategoriesModel>("IarChep.MeeShop", 1, 0, "CategoriesModel");
    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");
    qmlRegisterType<MeeShop::PackageManager>("IarChep.MeeShop", 1, 0, "PackageManager");
    qmlRegisterType<MeeShop::NokiaShape>("IarChep.MeeShop", 1, 0, "NokiaShape");
    qmlRegisterType<MeeShop::ProgressIndicator>("IarChep.MeeShop", 1, 0, "ProgressIndicator");
    qmlRegisterUncreatableType<MeeShop::ApplicationInfo>("IarChep.MeeShop", 1, 0, "ApplicationInfo", "ApplicationInfo is a read-only property!");
    qRegisterMetaType<MeeShop::ApplicationInfo*>();
    qRegisterMetaType<MeeShop::MeeShopApplicationModel*>();

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showExpanded();

    SwipeControl * swipeControl = new SwipeControl(&viewer, true);

    return app->exec();
}
