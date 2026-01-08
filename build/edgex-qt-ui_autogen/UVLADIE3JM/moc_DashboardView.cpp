/****************************************************************************
** Meta object code from reading C++ file 'DashboardView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/DashboardView.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DashboardView.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DashboardView_t {
    uint offsetsAndSizes[34];
    char stringdata0[14];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[8];
    char stringdata5[19];
    char stringdata6[9];
    char stringdata7[19];
    char stringdata8[9];
    char stringdata9[17];
    char stringdata10[7];
    char stringdata11[19];
    char stringdata12[9];
    char stringdata13[24];
    char stringdata14[14];
    char stringdata15[20];
    char stringdata16[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DashboardView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DashboardView_t qt_meta_stringdata_DashboardView = {
    {
        QT_MOC_LITERAL(0, 13),  // "DashboardView"
        QT_MOC_LITERAL(14, 7),  // "refresh"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 17),  // "onDevicesReceived"
        QT_MOC_LITERAL(41, 7),  // "devices"
        QT_MOC_LITERAL(49, 18),  // "onServicesReceived"
        QT_MOC_LITERAL(68, 8),  // "services"
        QT_MOC_LITERAL(77, 18),  // "onProfilesReceived"
        QT_MOC_LITERAL(96, 8),  // "profiles"
        QT_MOC_LITERAL(105, 16),  // "onEventsReceived"
        QT_MOC_LITERAL(122, 6),  // "events"
        QT_MOC_LITERAL(129, 18),  // "onReadingsReceived"
        QT_MOC_LITERAL(148, 8),  // "readings"
        QT_MOC_LITERAL(157, 23),  // "onNotificationsReceived"
        QT_MOC_LITERAL(181, 13),  // "notifications"
        QT_MOC_LITERAL(195, 19),  // "onIntervalsReceived"
        QT_MOC_LITERAL(215, 9)   // "intervals"
    },
    "DashboardView",
    "refresh",
    "",
    "onDevicesReceived",
    "devices",
    "onServicesReceived",
    "services",
    "onProfilesReceived",
    "profiles",
    "onEventsReceived",
    "events",
    "onReadingsReceived",
    "readings",
    "onNotificationsReceived",
    "notifications",
    "onIntervalsReceived",
    "intervals"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DashboardView[] = {

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
       7,    1,   69,    2, 0x08,    6 /* Private */,
       9,    1,   72,    2, 0x08,    8 /* Private */,
      11,    1,   75,    2, 0x08,   10 /* Private */,
      13,    1,   78,    2, 0x08,   12 /* Private */,
      15,    1,   81,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,    4,
    QMetaType::Void, QMetaType::QJsonArray,    6,
    QMetaType::Void, QMetaType::QJsonArray,    8,
    QMetaType::Void, QMetaType::QJsonArray,   10,
    QMetaType::Void, QMetaType::QJsonArray,   12,
    QMetaType::Void, QMetaType::QJsonArray,   14,
    QMetaType::Void, QMetaType::QJsonArray,   16,

       0        // eod
};

Q_CONSTINIT const QMetaObject DashboardView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DashboardView.offsetsAndSizes,
    qt_meta_data_DashboardView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DashboardView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DashboardView, std::true_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDevicesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onServicesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onProfilesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onEventsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onReadingsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onNotificationsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onIntervalsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>
    >,
    nullptr
} };

void DashboardView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DashboardView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->onDevicesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->onServicesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 3: _t->onProfilesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 4: _t->onEventsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 5: _t->onReadingsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 6: _t->onNotificationsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 7: _t->onIntervalsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *DashboardView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DashboardView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DashboardView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DashboardView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
