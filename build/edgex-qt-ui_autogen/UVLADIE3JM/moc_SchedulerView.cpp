/****************************************************************************
** Meta object code from reading C++ file 'SchedulerView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/SchedulerView.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SchedulerView.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_SchedulerView_t {
    uint offsetsAndSizes[28];
    char stringdata0[14];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[10];
    char stringdata5[18];
    char stringdata6[8];
    char stringdata7[21];
    char stringdata8[8];
    char stringdata9[8];
    char stringdata10[14];
    char stringdata11[17];
    char stringdata12[12];
    char stringdata13[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SchedulerView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SchedulerView_t qt_meta_stringdata_SchedulerView = {
    {
        QT_MOC_LITERAL(0, 13),  // "SchedulerView"
        QT_MOC_LITERAL(14, 7),  // "refresh"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 19),  // "onIntervalsReceived"
        QT_MOC_LITERAL(43, 9),  // "intervals"
        QT_MOC_LITERAL(53, 17),  // "onActionsReceived"
        QT_MOC_LITERAL(71, 7),  // "actions"
        QT_MOC_LITERAL(79, 20),  // "onOperationCompleted"
        QT_MOC_LITERAL(100, 7),  // "success"
        QT_MOC_LITERAL(108, 7),  // "message"
        QT_MOC_LITERAL(116, 13),  // "onAddInterval"
        QT_MOC_LITERAL(130, 16),  // "onDeleteInterval"
        QT_MOC_LITERAL(147, 11),  // "onAddAction"
        QT_MOC_LITERAL(159, 14)   // "onDeleteAction"
    },
    "SchedulerView",
    "refresh",
    "",
    "onIntervalsReceived",
    "intervals",
    "onActionsReceived",
    "actions",
    "onOperationCompleted",
    "success",
    "message",
    "onAddInterval",
    "onDeleteInterval",
    "onAddAction",
    "onDeleteAction"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SchedulerView[] = {

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
       7,    2,   69,    2, 0x08,    6 /* Private */,
      10,    0,   74,    2, 0x08,    9 /* Private */,
      11,    0,   75,    2, 0x08,   10 /* Private */,
      12,    0,   76,    2, 0x08,   11 /* Private */,
      13,    0,   77,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,    4,
    QMetaType::Void, QMetaType::QJsonArray,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject SchedulerView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SchedulerView.offsetsAndSizes,
    qt_meta_data_SchedulerView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SchedulerView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SchedulerView, std::true_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onIntervalsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onActionsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onOperationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAddInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SchedulerView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SchedulerView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->onIntervalsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->onActionsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 3: _t->onOperationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->onAddInterval(); break;
        case 5: _t->onDeleteInterval(); break;
        case 6: _t->onAddAction(); break;
        case 7: _t->onDeleteAction(); break;
        default: ;
        }
    }
}

const QMetaObject *SchedulerView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SchedulerView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SchedulerView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SchedulerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
