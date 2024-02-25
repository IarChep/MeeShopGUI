/**
 * SwipeControl class for Harmattan QML projects
 * Copyright (c) 2011 Thomas Perl <thp.io/about>
 * http://thp.io/2011/swipecontrol/
 * https://projects.developer.nokia.com/QtDrumkit/browser#qtdrumkit/QtDrumkit/qmlapplicationviewer
 **/

#include "swipecontrol.h"

#include <QX11Info>

#ifndef QT_SIMULATOR
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

SwipeControl::SwipeControl(QmlApplicationViewer *view, bool autoExpose)
    : QObject(view),
      _locked(true),
      _view(view)
{
    size.setWidth(QApplication::desktop()->screenGeometry().width());
    size.setHeight(QApplication::desktop()->screenGeometry().height());
    view->resize(size);

    if (autoExpose) {
        view->rootContext()->setContextProperty("swipeControl", this);
    }

    updateLockedState();
}

void SwipeControl::updateLockedState()
{
#ifndef QT_SIMULATOR
    QRect rect(0, 0, 0, 0);

    if (_locked) {
        rect.setRect(_view->x()+10,
                     _view->y()+64,
                     _view->width()-10-2,
                     _view->height());
        qWarning() << "View Size: " << rect;
    }

    unsigned int customRegion[] = {
        rect.x(),
        rect.y(),
        rect.width(),
        rect.height()
    };

    Display *dpy = QX11Info::display();
    Atom customRegionAtom = XInternAtom(dpy, "_MEEGOTOUCH_CUSTOM_REGION", False);
    XChangeProperty(dpy, _view->winId(), customRegionAtom,
                    XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<unsigned char*>(&customRegion[0]), 4);
#endif
}
