#include <QApplication>
#include <QScopedPointer>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeView>

#include "swipecontrol.h"

#include "openreposapi.h"
#include "MeeShopApplicationModel.h"
#include "MeeShopCategoriesModel.h"
#include "Gradienter.h"
#include "processmanager.h"
#include "nokiashape.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(new QApplication(argc, argv));

    qmlRegisterType<MeeShop::OpenReposApi>("IarChep.MeeShop", 1, 0, "OpenReposApi");
    qmlRegisterType<MeeShop::MeeShopApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel");
    qmlRegisterType<MeeShop::MeeShopCategoriesModel>("IarChep.MeeShop", 1, 0, "CategoriesModel");
    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");
    qmlRegisterType<MeeShop::ProcessManager>("IarChep.MeeShop", 1, 0, "ProcessManager");
    qmlRegisterType<MeeShop::NokiaShape>("IarChep.MeeShop", 1, 0, "NokiaShape");
    qRegisterMetaType<MeeShop::MeeShopApplicationModel*>();

    QScopedPointer<QDeclarativeView> viewer(new QDeclarativeView);
    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    SwipeControl * swipeControl = new SwipeControl(viewer.data(), true);

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));
    viewer->showFullScreen();

    return app->exec();
}
