#include <QApplication>
#include "qmlapplicationviewer.h"
#include "swipecontrol.h"

#include "information_processor.h"
#include "MeeShopApplicationModel.h"
#include "Gradienter.h"
#include "processmanager.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<MeeShop::information_processor>("IarChep.MeeShop", 1, 0, "MeeShopRepository");
    qmlRegisterType<MeeShop::MeeShopApplicationModel>("IarChep.MeeShop", 1, 0, "ApplicationModel");
    qmlRegisterType<MeeShop::Gradienter>("IarChep.MeeShop", 1, 0, "Gradienter");
    qmlRegisterType<MeeShop::ProcessManager>("IarChep.MeeShop", 1, 0, "ProcessManager");

    qRegisterMetaType<MeeShop::MeeShopApplicationModel*>();

    QmlApplicationViewer viewer;

    SwipeControl * swipeControl = new SwipeControl(&viewer, true);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/SwipePageStackWindow/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
