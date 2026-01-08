/****************************************************************************
** Meta object code from reading C++ file 'DeviceServicesView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/DeviceServicesView.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DeviceServicesView.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DeviceServicesView_t {
    uint offsetsAndSizes[28];
    char stringdata0[19];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[9];
    char stringdata5[9];
    char stringdata6[5];
    char stringdata7[14];
    char stringdata8[16];
    char stringdata9[14];
    char stringdata10[16];
    char stringdata11[21];
    char stringdata12[8];
    char stringdata13[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DeviceServicesView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DeviceServicesView_t qt_meta_stringdata_DeviceServicesView = {
    {
        QT_MOC_LITERAL(0, 18),  // "DeviceServicesView"
        QT_MOC_LITERAL(19, 7),  // "refresh"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 18),  // "onServicesReceived"
        QT_MOC_LITERAL(47, 8),  // "services"
        QT_MOC_LITERAL(56, 8),  // "onSearch"
        QT_MOC_LITERAL(65, 4),  // "text"
        QT_MOC_LITERAL(70, 13),  // "onEditService"
        QT_MOC_LITERAL(84, 15),  // "onDeleteService"
        QT_MOC_LITERAL(100, 13),  // "onLockService"
        QT_MOC_LITERAL(114, 15),  // "onUnlockService"
        QT_MOC_LITERAL(130, 20),  // "onOperationCompleted"
        QT_MOC_LITERAL(151, 7),  // "success"
        QT_MOC_LITERAL(159, 7)   // "message"
    },
    "DeviceServicesView",
    "refresh",
    "",
    "onServicesReceived",
    "services",
    "onSearch",
    "text",
    "onEditService",
    "onDeleteService",
    "onLockService",
    "onUnlockService",
    "onOperationCompleted",
    "success",
    "message"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DeviceServicesView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    1,   63,    2, 0x08,    2 /* Private */,
       5,    1,   66,    2, 0x08,    4 /* Private */,
       7,    0,   69,    2, 0x08,    6 /* Private */,
       8,    0,   70,    2, 0x08,    7 /* Private */,
       9,    0,   71,    2, 0x08,    8 /* Private */,
      10,    0,   72,    2, 0x08,    9 /* Private */,
      11,    2,   73,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   12,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject DeviceServicesView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DeviceServicesView.offsetsAndSizes,
    qt_meta_data_DeviceServicesView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DeviceServicesView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DeviceServicesView, std::true_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onServicesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onSearch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onEditService'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteService'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLockService'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUnlockService'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOperationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void DeviceServicesView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeviceServicesView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->onServicesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->onSearch((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onEditService(); break;
        case 4: _t->onDeleteService(); break;
        case 5: _t->onLockService(); break;
        case 6: _t->onUnlockService(); break;
        case 7: _t->onOperationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *DeviceServicesView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeviceServicesView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceServicesView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DeviceServicesView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
