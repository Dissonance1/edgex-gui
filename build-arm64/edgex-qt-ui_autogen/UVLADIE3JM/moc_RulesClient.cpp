/****************************************************************************
** Meta object code from reading C++ file 'RulesClient.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/RulesClient.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RulesClient.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_RulesClient_t {
    uint offsetsAndSizes[24];
    char stringdata0[12];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[14];
    char stringdata5[6];
    char stringdata6[19];
    char stringdata7[3];
    char stringdata8[7];
    char stringdata9[19];
    char stringdata10[8];
    char stringdata11[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RulesClient_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RulesClient_t qt_meta_stringdata_RulesClient = {
    {
        QT_MOC_LITERAL(0, 11),  // "RulesClient"
        QT_MOC_LITERAL(12, 15),  // "streamsReceived"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 7),  // "streams"
        QT_MOC_LITERAL(37, 13),  // "rulesReceived"
        QT_MOC_LITERAL(51, 5),  // "rules"
        QT_MOC_LITERAL(57, 18),  // "ruleStatusReceived"
        QT_MOC_LITERAL(76, 2),  // "id"
        QT_MOC_LITERAL(79, 6),  // "status"
        QT_MOC_LITERAL(86, 18),  // "operationCompleted"
        QT_MOC_LITERAL(105, 7),  // "success"
        QT_MOC_LITERAL(113, 7)   // "message"
    },
    "RulesClient",
    "streamsReceived",
    "",
    "streams",
    "rulesReceived",
    "rules",
    "ruleStatusReceived",
    "id",
    "status",
    "operationCompleted",
    "success",
    "message"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RulesClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       4,    1,   41,    2, 0x06,    3 /* Public */,
       6,    2,   44,    2, 0x06,    5 /* Public */,
       9,    2,   49,    2, 0x06,    8 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QJsonArray,    3,
    QMetaType::Void, QMetaType::QJsonArray,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,    7,    8,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   10,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject RulesClient::staticMetaObject = { {
    QMetaObject::SuperData::link<EdgexClientBase::staticMetaObject>(),
    qt_meta_stringdata_RulesClient.offsetsAndSizes,
    qt_meta_data_RulesClient,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RulesClient_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RulesClient, std::true_type>,
        // method 'streamsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'rulesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'ruleStatusReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'operationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void RulesClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RulesClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->streamsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 1: _t->rulesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->ruleStatusReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 3: _t->operationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RulesClient::*)(const QJsonArray & );
            if (_t _q_method = &RulesClient::streamsReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RulesClient::*)(const QJsonArray & );
            if (_t _q_method = &RulesClient::rulesReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RulesClient::*)(const QString & , const QJsonObject & );
            if (_t _q_method = &RulesClient::ruleStatusReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RulesClient::*)(bool , const QString & );
            if (_t _q_method = &RulesClient::operationCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *RulesClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RulesClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RulesClient.stringdata0))
        return static_cast<void*>(this);
    return EdgexClientBase::qt_metacast(_clname);
}

int RulesClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EdgexClientBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RulesClient::streamsReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RulesClient::rulesReceived(const QJsonArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RulesClient::ruleStatusReceived(const QString & _t1, const QJsonObject & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RulesClient::operationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
