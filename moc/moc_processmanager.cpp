/****************************************************************************
** Meta object code from reading C++ file 'processmanager.h'
**
** Created: Fri 29. Mar 22:42:11 2024
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../processmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'processmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MeeShop__ProcessManager[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       6,  114, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   25,   24,   24, 0x05,
      55,   25,   24,   24, 0x05,
      83,   24,   24,   24, 0x05,
     107,   24,   24,   24, 0x05,
     130,   24,   24,   24, 0x05,
     158,   24,   24,   24, 0x05,
     188,   24,   24,   24, 0x05,
     217,   24,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
     251,   24,   24,   24, 0x0a,
     281,  273,   24,   24, 0x0a,
     311,  273,  306,   24, 0x0a,
     333,   24,   24,   24, 0x0a,
     361,  341,   24,   24, 0x08,
     411,   24,   24,   24, 0x08,
     435,   24,   24,   24, 0x08,
     469,  463,   24,   24, 0x08,
     514,  341,   24,   24, 0x08,
     570,   24,   24,   24, 0x08,
     600,   24,   24,   24, 0x08,
     634,  463,   24,   24, 0x08,

 // properties: name, type, flags
     693,  685, 0x0a495001,
     706,  685, 0x0a495001,
     722,  718, 0x02495001,
     739,  685, 0x0a495001,
     758,  685, 0x0a495001,
     776,  685, 0x0a495001,

 // properties: notify_signal_id
       2,
       3,
       4,
       5,
       6,
       7,

       0        // eod
};

static const char qt_meta_stringdata_MeeShop__ProcessManager[] = {
    "MeeShop::ProcessManager\0\0sucsess\0"
    "update_finished(bool)\0installation_finished(bool)\0"
    "update_output_changed()\0update_error_changed()\0"
    "update_percentage_changed()\0"
    "installation_output_changed()\0"
    "installation_error_changed()\0"
    "installation_percentage_changed()\0"
    "update_repositories()\0package\0"
    "install_package(QString)\0bool\0"
    "is_installed(QString)\0reset()\0"
    "exitCode,exitStatus\0"
    "update_process_finished(int,QProcess::ExitStatus)\0"
    "process_update_output()\0"
    "process_update_read_error()\0error\0"
    "process_update_error(QProcess::ProcessError)\0"
    "installation_process_finished(int,QProcess::ExitStatus)\0"
    "process_installation_output()\0"
    "process_installation_read_error()\0"
    "process_installation_error(QProcess::ProcessError)\0"
    "QString\0updateOutput\0updateError\0int\0"
    "updatePercentage\0installationOutput\0"
    "installationError\0installationPercentage\0"
};

const QMetaObject MeeShop::ProcessManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MeeShop__ProcessManager,
      qt_meta_data_MeeShop__ProcessManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MeeShop::ProcessManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MeeShop::ProcessManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MeeShop::ProcessManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MeeShop__ProcessManager))
        return static_cast<void*>(const_cast< ProcessManager*>(this));
    return QObject::qt_metacast(_clname);
}

int MeeShop::ProcessManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: update_finished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: installation_finished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: update_output_changed(); break;
        case 3: update_error_changed(); break;
        case 4: update_percentage_changed(); break;
        case 5: installation_output_changed(); break;
        case 6: installation_error_changed(); break;
        case 7: installation_percentage_changed(); break;
        case 8: update_repositories(); break;
        case 9: install_package((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: { bool _r = is_installed((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: reset(); break;
        case 12: update_process_finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 13: process_update_output(); break;
        case 14: process_update_read_error(); break;
        case 15: process_update_error((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 16: installation_process_finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 17: process_installation_output(); break;
        case 18: process_installation_read_error(); break;
        case 19: process_installation_error((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 20;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = get_update_output(); break;
        case 1: *reinterpret_cast< QString*>(_v) = get_update_error(); break;
        case 2: *reinterpret_cast< int*>(_v) = get_update_percentage(); break;
        case 3: *reinterpret_cast< QString*>(_v) = get_installation_output(); break;
        case 4: *reinterpret_cast< QString*>(_v) = get_installation_error(); break;
        case 5: *reinterpret_cast< QString*>(_v) = get_installation_percentage(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MeeShop::ProcessManager::update_finished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MeeShop::ProcessManager::installation_finished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MeeShop::ProcessManager::update_output_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void MeeShop::ProcessManager::update_error_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void MeeShop::ProcessManager::update_percentage_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void MeeShop::ProcessManager::installation_output_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void MeeShop::ProcessManager::installation_error_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void MeeShop::ProcessManager::installation_percentage_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}
QT_END_MOC_NAMESPACE
