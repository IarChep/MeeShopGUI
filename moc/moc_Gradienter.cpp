/****************************************************************************
** Meta object code from reading C++ file 'Gradienter.h'
**
** Created: Thu 28. Mar 16:31:30 2024
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Gradienter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Gradienter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MeeShop__Gradienter[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      44,   33,   21,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MeeShop__Gradienter[] = {
    "MeeShop::Gradienter\0\0QStringList\0"
    "image_path\0get_gradient_colors(QString)\0"
};

const QMetaObject MeeShop::Gradienter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MeeShop__Gradienter,
      qt_meta_data_MeeShop__Gradienter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MeeShop::Gradienter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MeeShop::Gradienter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MeeShop::Gradienter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MeeShop__Gradienter))
        return static_cast<void*>(const_cast< Gradienter*>(this));
    return QObject::qt_metacast(_clname);
}

int MeeShop::Gradienter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QStringList _r = get_gradient_colors((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
