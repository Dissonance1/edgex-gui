/****************************************************************************
** Meta object code from reading C++ file 'SupportClient.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/SupportClient.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SupportClient.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_SupportClient_t {
    uint offsetsAndSizes[26];
    char stringdata0[14];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[22];
    char stringdata5[14];
    char stringdata6[18];
    char stringdata7[10];
    char stringdata8[24];
    char stringdata9[8];
    char stringdata10[19];
    char stringdata11[8];
    char stringdata12[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SupportClient_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SupportClient_t qt_meta_stringdata_SupportClient = {
    {
        QT_MOC_LITERAL(0, 13),  // "SupportClient"
        QT_MOC_LITERAL(14, 21),  // "notificationsReceived"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 13),  // "notifications"
        QT_MOC_LITERAL(51, 21),  // "subscriptionsReceived"
        QT_MOC_LITERAL(73, 13),  // "subscriptions"
        QT_MOC_LITERAL(87, 17),  // "intervalsReceived"
        QT_MOC_LITERAL(105, 9),  // "intervals"
        QT_MOC_LITERAL(115, 23),  // "intervalActionsReceived"
        QT_MOC_LITERAL(139, 7),  // "actions"
        QT_MOC_LITERAL(147, 18),  // "operationCompleted"
        QT_MOC_LITERAL(166, 7),  // "success"
        QT_MOC_LITERAL(174, 7)   // "message"
    },
    "SupportClient",
    "notificationsReceived",
    "",
    "notifications",
    "subscriptionsReceived",
    "subscriptions",
    "intervalsReceived",
    "intervals",
    "intervalActionsReceived",
    "actions",
    "operationCompleted",
    "success",
    "message"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SupportClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       4,    1,   47,    2, 0x06,    3 /* Public */,
       6,    1,   50,    2, 0x06,    5 /* Public */,
       8,    1,   53,    2, 0x06,    7 /* Public */,
      10,    2,   56,    2, 0x06,    9 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QJsonArray,    3,
    QMetaType::Void, QMetaType::QJsonArray,    5,
    QMetaType::Void, QMetaType::QJsonArray,    7,
    QMetaType::Void, QMetaType::QJsonArray,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   11,   12,

       0        // eod
};

Q_CONSTINIT const QMetaObject SupportClient::staticMetaObject = { {
    QMetaObject::SuperData::link<EdgexClientBase::staticMetaObject>(),
    qt_meta_stringdata_SupportClient.offsetsAndSizes,
    qt_meta_data_SupportClient,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SupportClient_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SupportClient, std::true_type>,
        // method 'notificationsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'subscriptionsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'intervalsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'intervalActionsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'operationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void SupportClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SupportClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->notificationsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 1: _t->subscriptionsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->intervalsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 3: _t->intervalActionsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 4: _t->operationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SupportClient::*)(const QJsonArray & );
            if (_t _q_method = &SupportClient::notificationsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SupportClient::*)(const QJsonArray & );
            if (_t _q_method = &SupportClient::subscriptionsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SupportClient::*)(const QJsonArray & );
            if (_t _q_method = &SupportClient::intervalsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SupportClient::*)(const QJsonArray & );
            if (_t _q_method = &SupportClient::intervalActionsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SupportClient::*)(bool , const QString & );
            if (_t _q_method = &SupportClient::operationCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *SupportClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SupportClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SupportClient.stringdata0))
        return static_cast<void*>(this);
    return EdgexClientBase::qt_metacast(_clname);
}

int SupportClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EdgexClientBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SupportClient::notificationsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SupportClient::subscriptionsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SupportClient::intervalsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SupportClient::intervalActionsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SupportClient::operationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
