#ifndef _SWIPECONTROL_H
#define _SWIPECONTROL_H

/*
 * SwipeControl class for Harmattan QML projects
 * Copyright (c) 2011 Thomas Perl <thp.io/about>
 * http://thp.io/2011/swipecontrol/
 **/

#include <QtCore>
#include <QtGui>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include <QDebug>

class SwipeControl : public QObject
{
    Q_OBJECT

private:
    bool _locked;
    QPointF _posIni;
    QmlApplicationViewer *_view;
    QSize size;

    void updateLockedState();

public:
    SwipeControl(QmlApplicationViewer *view, bool autoExpose=true);

    bool locked() const { return _locked; }

    void setLocked(bool locked=true) {
        if (_locked != locked) {
            _locked = locked;
            updateLockedState();
            emit lockedChanged();
        }
    }

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)

signals:
    void lockedChanged();
};


#endif
