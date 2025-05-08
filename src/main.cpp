#include <QApplication>
#include <QScopedPointer>

#include "qmlapplicationviewer/qmlapplicationviewer.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>

#include "tools/openreposapi.h"
#include "models/applicationmodel.h"
#include "models/categoriesmodel.h"
#include "qml_elements/gradienter.h"
#include "tools/packagemanager.h"
#include "qml_elements/nokiashape.h"
#include "qml_elements/progressindicator.h"
#include "tools/notifyer.h"
#include <QTextCodec>



Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);
    QTextCodec::setCodecForCStrings(utfCodec);
    QTextCodec::setCodecForTr(utfCodec);

    MeeShop::OpenReposApi api(app.data());
    MeeShop::PackageManager packageManager(app.data());
    MeeShop::Gradienter gradienter(app.data());
    MeeShop::Notifyer notifyer(app.data());

    packageManager.cacheInstalledPackages();
    packageManager.cacheEnabledRepositories();

    qmlRegisterUncreatableType<MeeShop::ApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel", "ApplicationModel is a read-only type!");
    qmlRegisterUncreatableType<MeeShop::CategoriesModel>("IarChep.MeeShop", 1, 0, "CategoriesModel", "CategoriesModel is a read-only type!");
    qmlRegisterUncreatableType<MeeShop::PackageManager>("IarChep.MeeShop", 1, 0, "PackageManager", "Only for ENUM!");

    qmlRegisterType<MeeShop::NokiaShape>("IarChep.MeeShop", 1, 0, "NokiaShape");
    qmlRegisterType<MeeShop::ProgressIndicator>("IarChep.MeeShop", 1, 0, "ProgressIndicator");

    qRegisterMetaType<MeeShop::ApplicationModel*>();
    //qRegisterMetaType<MeeShop::PackageManager::InstallationStatus>();

    QmlApplicationViewer viewer;
    QDeclarativeContext* rootContext = viewer.rootContext();
    rootContext->setContextProperty("api", &api);
    rootContext->setContextProperty("packageManager", &packageManager);
    rootContext->setContextProperty("gradienter", &gradienter);
    rootContext->setContextProperty("notifyer", &notifyer);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
