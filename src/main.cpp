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
#include "models/installedappsmodel.h"
#include "qml_elements/gradienter.h"
#include "tools/installationqueue.h"
#include "tools/packageutils.h"
#include "qml_elements/nokiashape.h"
#include "qml_elements/progressindicator.h"
#include "tools/notifyer.h"
#include "tools/mediaopener.h"
#include <QTextCodec>
#include "tools/settings.h"



Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QCoreApplication::setApplicationName("MeeShop");
    QCoreApplication::setOrganizationName("IarChep");

    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);
    QTextCodec::setCodecForCStrings(utfCodec);
    QTextCodec::setCodecForTr(utfCodec);

    MeeShop::OpenReposApi api(app.data());
    MeeShop::InstallationQueue queue(app.data());
    MeeShop::Notifyer notifyer(app.data());
    MeeShop::InstalledAppsModel installedApps(app.data());
    MeeShop::MediaOpener mediaOpener(app.data());

    queue.cacheInstalledPackages();
    queue.cacheEnabledRepositories();

    qmlRegisterUncreatableType<MeeShop::ApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel", "ApplicationModel is a read-only type!");
    qmlRegisterUncreatableType<MeeShop::CategoriesModel>("IarChep.MeeShop", 1, 0, "CategoriesModel", "CategoriesModel is a read-only type!");
    qmlRegisterUncreatableType<MeeShop::InstallationQueue>("IarChep.MeeShop", 1, 0, "InstallationQueue", "Use the 'queue' context object; type exposed for enums only");

    qmlRegisterType<MeeShop::NokiaShape>("IarChep.MeeShop", 1, 0, "NokiaShape");
    qmlRegisterType<MeeShop::ProgressIndicator>("IarChep.MeeShop", 1, 0, "ProgressIndicator");
    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");

    qRegisterMetaType<MeeShop::ApplicationModel*>();

    QmlApplicationViewer viewer;
    QDeclarativeContext* rootContext = viewer.rootContext();
    rootContext->setContextProperty("api", &api);
    rootContext->setContextProperty("queue", &queue);
    rootContext->setContextProperty("notifyer", &notifyer);
    rootContext->setContextProperty("installedApps", &installedApps);
    rootContext->setContextProperty("mediaOpener", &mediaOpener);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
