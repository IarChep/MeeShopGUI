/****************************************************************************
** Meta object code from reading C++ file 'information_processor.h'
**
** Created: Fri 15. Mar 13:06:09 2024
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../information_processor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'information_processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MeeShop__information_processor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       1,   59, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   31,   31,   31, 0x05,
      63,   31,   31,   31, 0x05,
      85,   79,   31,   31, 0x05,
     108,   79,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     137,  131,   31,   31, 0x08,
     163,   31,   31,   31, 0x08,
     179,   31,   31,   31, 0x08,

 // methods: signature, parameters, type, tag, flags
     194,  190,   31,   31, 0x02,
     221,  190,   31,   31, 0x02,

 // properties: name, type, flags
     279,  245, 0x00495009,

 // properties: notify_signal_id
       1,

       0        // eod
};

static const char qt_meta_stringdata_MeeShop__information_processor[] = {
    "MeeShop::information_processor\0\0"
    "information_loading_finished()\0"
    "model_changed()\0error\0loading_error(QString)\0"
    "parsing_error(QString)\0reply\0"
    "parse_xml(QNetworkReply*)\0process_reply()\0"
    "finilase()\0xml\0load_applications(QString)\0"
    "load_rss_feeds(QString)\0"
    "MeeShop::MeeShopApplicationModel*\0"
    "model\0"
};

const QMetaObject MeeShop::information_processor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MeeShop__information_processor,
      qt_meta_data_MeeShop__information_processor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MeeShop::information_processor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MeeShop::information_processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MeeShop::information_processor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MeeShop__information_processor))
        return static_cast<void*>(const_cast< information_processor*>(this));
    return QObject::qt_metacast(_clname);
}

int MeeShop::information_processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: information_loading_finished(); break;
        case 1: model_changed(); break;
        case 2: loading_error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: parsing_error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: parse_xml((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: process_reply(); break;
        case 6: finilase(); break;
        case 7: load_applications((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: load_rss_feeds((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< MeeShop::MeeShopApplicationModel**>(_v) = get_final_list(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MeeShop::information_processor::information_loading_finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MeeShop::information_processor::model_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MeeShop::information_processor::loading_error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MeeShop::information_processor::parsing_error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
