#include <QApplication>
#include <QScopedPointer>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeView>

#include "swipecontrol.h"

#include "information_processor.h"
#include "MeeShopApplicationModel.h"
#include "Gradienter.h"
#include "processmanager.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(new QApplication(argc, argv));

    MeeShop::ProcessManager pm;
    pm.check_root();

    qmlRegisterType<MeeShop::information_processor>("IarChep.MeeShop", 1, 0, "MeeShopRepository");
    qmlRegisterType<MeeShop::MeeShopApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel");
    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");
    qmlRegisterType<MeeShop::ProcessManager>("IarChep.MeeShop", 1, 0, "ProcessManager");
    qRegisterMetaType<MeeShop::MeeShopApplicationModel*>();

    QScopedPointer<QDeclarativeView> viewer(new QDeclarativeView);
    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    SwipeControl * swipeControl = new SwipeControl(viewer.data(), true);

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));
    viewer->showFullScreen();

    return app->exec();
}
