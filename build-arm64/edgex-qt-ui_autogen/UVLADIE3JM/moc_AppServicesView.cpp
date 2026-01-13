/****************************************************************************
** Meta object code from reading C++ file 'AppServicesView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/AppServicesView.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppServicesView.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_AppServicesView_t {
    uint offsetsAndSizes[28];
    char stringdata0[16];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[9];
    char stringdata5[13];
    char stringdata6[17];
    char stringdata7[7];
    char stringdata8[21];
    char stringdata9[8];
    char stringdata10[8];
    char stringdata11[9];
    char stringdata12[5];
    char stringdata13[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AppServicesView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AppServicesView_t qt_meta_stringdata_AppServicesView = {
    {
        QT_MOC_LITERAL(0, 15),  // "AppServicesView"
        QT_MOC_LITERAL(16, 7),  // "refresh"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 21),  // "onAppServicesReceived"
        QT_MOC_LITERAL(47, 8),  // "services"
        QT_MOC_LITERAL(56, 12),  // "onViewConfig"
        QT_MOC_LITERAL(69, 16),  // "onConfigReceived"
        QT_MOC_LITERAL(86, 6),  // "config"
        QT_MOC_LITERAL(93, 20),  // "onOperationCompleted"
        QT_MOC_LITERAL(114, 7),  // "success"
        QT_MOC_LITERAL(122, 7),  // "message"
        QT_MOC_LITERAL(130, 8),  // "onSearch"
        QT_MOC_LITERAL(139, 4),  // "text"
        QT_MOC_LITERAL(144, 17)   // "onAddFromTemplate"
    },
    "AppServicesView",
    "refresh",
    "",
    "onAppServicesReceived",
    "services",
    "onViewConfig",
    "onConfigReceived",
    "config",
    "onOperationCompleted",
    "success",
    "message",
    "onSearch",
    "text",
    "onAddFromTemplate"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AppServicesView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    1,   57,    2, 0x08,    2 /* Private */,
       5,    0,   60,    2, 0x08,    4 /* Private */,
       6,    1,   61,    2, 0x08,    5 /* Private */,
       8,    2,   64,    2, 0x08,    7 /* Private */,
      11,    1,   69,    2, 0x08,   10 /* Private */,
      13,    0,   72,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject AppServicesView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_AppServicesView.offsetsAndSizes,
    qt_meta_data_AppServicesView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AppServicesView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AppServicesView, std::true_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAppServicesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onViewConfig'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConfigReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onOperationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onSearch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAddFromTemplate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void AppServicesView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AppServicesView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->onAppServicesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->onViewConfig(); break;
        case 3: _t->onConfigReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->onOperationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->onSearch((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onAddFromTemplate(); break;
        default: ;
        }
    }
}

const QMetaObject *AppServicesView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppServicesView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AppServicesView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AppServicesView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
