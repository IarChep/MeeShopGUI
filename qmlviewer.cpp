/**
 * Copyright (c) 2011 Nokia Corporation.
 * All rights reserved.
 *
 * Part of the QtDrumkit
 *
 * For the applicable distribution terms see the license text file included in
 * the distribution.


 https://projects.developer.nokia.com/QtDrumkit/browser#qtdrumkit/QtDrumkit/qmlapplicationviewer

 */
#include "qmlviewer.h"
QmlViewer *QmlViewer::m_instance;
/*!
  \class QmlViewer
  \brief A wrapper for QmlApplicationViewer to get access to touch events.
         The signal touchEventReceived is grabbed by an instance of TouchEvents class,
         which is created from QML. For this to work, it needs an access to
         QmlViewer instance, therefore the static instance member.
*/
QmlViewer::QmlViewer()
{
    grabGesture(Qt::SwipeGesture);
    // We'd like to have touch events.
    setAttribute(Qt::WA_AcceptTouchEvents);
    m_instance = this;
}
QmlViewer *QmlViewer::instance() {
    return m_instance;
}

bool QmlViewer::event(QEvent *event)
{
    // Grab touch events and signal appropriately.
    QEvent::Type type = event->type();

    if(type == QEvent::Gesture || type == QEvent::GestureOverride){
        qWarning() << "Gesture";
        QGestureEvent *ge = static_cast<QGestureEvent*>(event);
        QGesture *swipe = ge->gesture(Qt::SwipeGesture);
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    }

    if(type == QEvent::TouchBegin)
    {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        QTouchEvent::TouchPoint tp = te->touchPoints().at(0);
        emit touchPressed(tp.pos().y(), tp.pos().x());
        return true;
    }
    else if(type == QEvent::TouchUpdate)
    {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        QTouchEvent::TouchPoint tp = te->touchPoints().at(0);
        emit touchMoved(tp.pos().x(), tp.pos().y());
        return true;
    }
    else if(type == QEvent::TouchEnd)
    {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        QTouchEvent::TouchPoint tp = te->touchPoints().at(0);
        emit touchReleased(tp.pos().x(), tp.pos().y());
        return true;
    }
    else
    {
        return QmlApplicationViewer::event(event);
    }

    /*

    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        QTouchEvent::TouchPoint tp = te->touchPoints().at(0);
        Qt::TouchPointState state = tp.state();
        QPointF pos = tp.pos();

        qWarning() << pos;

        if(state == Qt::TouchPointPressed)
            emit touchPressed(pos.x(), pos.y());
        else if(state == Qt::TouchPointMoved)
            emit touchMoved(pos.x(), pos.y());
        else if(state == Qt::TouchPointReleased)
            emit touchReleased(pos.x(), pos.y());

        if (state == Qt::TouchPointPressed || state == Qt::TouchPointMoved) {
            emit touchEventReceived(pos.x(), pos.y());
        }
        /*foreach (QTouchEvent::TouchPoint tp, te->touchPoints()) {
            if (tp.state() == Qt::TouchPointPressed || tp.state() == Qt::TouchPointMoved) {
                emit touchEventReceived(tp.pos().x(), tp.pos().y());
            }
        }*

        return true;
    }
    */
    //return QmlApplicationViewer::event(event);
}
