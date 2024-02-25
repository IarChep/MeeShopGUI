#include <QApplication>
#include "qmlapplicationviewer.h"
#include "swipecontrol.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;

    SwipeControl * swipeControl = new SwipeControl(&viewer, true);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/SwipePageStackWindow/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
