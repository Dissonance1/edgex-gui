/****************************************************************************
** Meta object code from reading C++ file 'CoreDataClient.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/CoreDataClient.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CoreDataClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_CoreDataClient_t {
    uint offsetsAndSizes[18];
    char stringdata0[15];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[17];
    char stringdata5[9];
    char stringdata6[19];
    char stringdata7[8];
    char stringdata8[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CoreDataClient_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CoreDataClient_t qt_meta_stringdata_CoreDataClient = {
    {
        QT_MOC_LITERAL(0, 14),  // "CoreDataClient"
        QT_MOC_LITERAL(15, 14),  // "eventsReceived"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 6),  // "events"
        QT_MOC_LITERAL(38, 16),  // "readingsReceived"
        QT_MOC_LITERAL(55, 8),  // "readings"
        QT_MOC_LITERAL(64, 18),  // "operationCompleted"
        QT_MOC_LITERAL(83, 7),  // "success"
        QT_MOC_LITERAL(91, 7)   // "message"
    },
    "CoreDataClient",
    "eventsReceived",
    "",
    "events",
    "readingsReceived",
    "readings",
    "operationCompleted",
    "success",
    "message"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CoreDataClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x06,    1 /* Public */,
       4,    1,   35,    2, 0x06,    3 /* Public */,
       6,    2,   38,    2, 0x06,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QJsonArray,    3,
    QMetaType::Void, QMetaType::QJsonArray,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,

       0        // eod
};

Q_CONSTINIT const QMetaObject CoreDataClient::staticMetaObject = { {
    QMetaObject::SuperData::link<EdgexClientBase::staticMetaObject>(),
    qt_meta_stringdata_CoreDataClient.offsetsAndSizes,
    qt_meta_data_CoreDataClient,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CoreDataClient_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CoreDataClient, std::true_type>,
        // method 'eventsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'readingsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'operationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void CoreDataClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CoreDataClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->eventsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 1: _t->readingsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->operationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CoreDataClient::*)(const QJsonArray & );
            if (_t _q_method = &CoreDataClient::eventsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CoreDataClient::*)(const QJsonArray & );
            if (_t _q_method = &CoreDataClient::readingsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CoreDataClient::*)(bool , const QString & );
            if (_t _q_method = &CoreDataClient::operationCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *CoreDataClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreDataClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CoreDataClient.stringdata0))
        return static_cast<void*>(this);
    return EdgexClientBase::qt_metacast(_clname);
}

int CoreDataClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EdgexClientBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CoreDataClient::eventsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CoreDataClient::readingsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CoreDataClient::operationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
