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
#include <QTextCodec>



Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(utfCodec);          // Локаль
        QTextCodec::setCodecForCStrings(utfCodec);        // C-строки (QString::fromUtf8 и т.д.)
        QTextCodec::setCodecForTr(utfCodec);

    MeeShop::OpenReposApi api(app.data());
    MeeShop::PackageManager packageManager(app.data());
    MeeShop::Gradienter gradienter(app.data());

        packageManager.printInstalledPackages();

    qmlRegisterUncreatableType<MeeShop::MeeShopApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel", "ApplicationModel is a read-only type!");
    qmlRegisterUncreatableType<MeeShop::MeeShopCategoriesModel>("IarChep.MeeShop", 1, 0, "CategoriesModel", "CategoriesModel is a read-only type!");

    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");
    qmlRegisterType<MeeShop::NokiaShape>("IarChep.MeeShop", 1, 0, "NokiaShape");
    qmlRegisterType<MeeShop::ProgressIndicator>("IarChep.MeeShop", 1, 0, "ProgressIndicator");
    qmlRegisterUncreatableType<MeeShop::ApplicationInfo>("IarChep.MeeShop", 1, 0, "ApplicationInfo", "ApplicationInfo is a read-only property!");
    qRegisterMetaType<MeeShop::ApplicationInfo*>();
    qRegisterMetaType<MeeShop::MeeShopApplicationModel*>();

    QmlApplicationViewer viewer;

    viewer.rootContext()->setContextProperty("api", &api);
    viewer.rootContext()->setContextProperty("packageManager", &packageManager);
    viewer.rootContext()->setContextProperty("gradienter", &gradienter);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showExpanded();


    //packageManager.cacheInstalledApplications();

    SwipeControl * swipeControl = new SwipeControl(&viewer, true);

    return app->exec();
}
