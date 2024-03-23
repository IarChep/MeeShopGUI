/****************************************************************************
** Meta object code from reading C++ file 'qmlviewer.h'
**
** Created: Wed 28. Feb 21:55:46 2024
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qmlviewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmlviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QmlViewer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   11,   10,   10, 0x05,
      51,   11,   10,   10, 0x05,
      78,   11,   10,   10, 0x05,
     102,   11,   10,   10, 0x05,
     142,  134,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QmlViewer[] = {
    "QmlViewer\0\0mouseX,mouseY\0"
    "touchPressed(qreal,qreal)\0"
    "touchReleased(qreal,qreal)\0"
    "touchMoved(qreal,qreal)\0"
    "touchEventReceived(qreal,qreal)\0gesture\0"
    "swipeTriggered(QSwipeGesture*)\0"
};

const QMetaObject QmlViewer::staticMetaObject = {
    { &QmlApplicationViewer::staticMetaObject, qt_meta_stringdata_QmlViewer,
      qt_meta_data_QmlViewer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QmlViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QmlViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QmlViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QmlViewer))
        return static_cast<void*>(const_cast< QmlViewer*>(this));
    return QmlApplicationViewer::qt_metacast(_clname);
}

int QmlViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QmlApplicationViewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: touchPressed((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 1: touchReleased((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 2: touchMoved((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 3: touchEventReceived((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 4: swipeTriggered((*reinterpret_cast< QSwipeGesture*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QmlViewer::touchPressed(qreal _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QmlViewer::touchReleased(qreal _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QmlViewer::touchMoved(qreal _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QmlViewer::touchEventReceived(qreal _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QmlViewer::swipeTriggered(QSwipeGesture * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
