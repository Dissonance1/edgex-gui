/****************************************************************************
** Meta object code from reading C++ file 'RulesEngineView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/RulesEngineView.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RulesEngineView.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_RulesEngineView_t {
    uint offsetsAndSizes[42];
    char stringdata0[16];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[8];
    char stringdata5[16];
    char stringdata6[6];
    char stringdata7[21];
    char stringdata8[3];
    char stringdata9[7];
    char stringdata10[21];
    char stringdata11[8];
    char stringdata12[8];
    char stringdata13[12];
    char stringdata14[15];
    char stringdata15[10];
    char stringdata16[13];
    char stringdata17[12];
    char stringdata18[11];
    char stringdata19[10];
    char stringdata20[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RulesEngineView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RulesEngineView_t qt_meta_stringdata_RulesEngineView = {
    {
        QT_MOC_LITERAL(0, 15),  // "RulesEngineView"
        QT_MOC_LITERAL(16, 7),  // "refresh"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 17),  // "onStreamsReceived"
        QT_MOC_LITERAL(43, 7),  // "streams"
        QT_MOC_LITERAL(51, 15),  // "onRulesReceived"
        QT_MOC_LITERAL(67, 5),  // "rules"
        QT_MOC_LITERAL(73, 20),  // "onRuleStatusReceived"
        QT_MOC_LITERAL(94, 2),  // "id"
        QT_MOC_LITERAL(97, 6),  // "status"
        QT_MOC_LITERAL(104, 20),  // "onOperationCompleted"
        QT_MOC_LITERAL(125, 7),  // "success"
        QT_MOC_LITERAL(133, 7),  // "message"
        QT_MOC_LITERAL(141, 11),  // "onAddStream"
        QT_MOC_LITERAL(153, 14),  // "onDeleteStream"
        QT_MOC_LITERAL(168, 9),  // "onAddRule"
        QT_MOC_LITERAL(178, 12),  // "onDeleteRule"
        QT_MOC_LITERAL(191, 11),  // "onStartRule"
        QT_MOC_LITERAL(203, 10),  // "onStopRule"
        QT_MOC_LITERAL(214, 9),  // "onViewSql"
        QT_MOC_LITERAL(224, 13)   // "onViewMetrics"
    },
    "RulesEngineView",
    "refresh",
    "",
    "onStreamsReceived",
    "streams",
    "onRulesReceived",
    "rules",
    "onRuleStatusReceived",
    "id",
    "status",
    "onOperationCompleted",
    "success",
    "message",
    "onAddStream",
    "onDeleteStream",
    "onAddRule",
    "onDeleteRule",
    "onStartRule",
    "onStopRule",
    "onViewSql",
    "onViewMetrics"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RulesEngineView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    1,   93,    2, 0x08,    2 /* Private */,
       5,    1,   96,    2, 0x08,    4 /* Private */,
       7,    2,   99,    2, 0x08,    6 /* Private */,
      10,    2,  104,    2, 0x08,    9 /* Private */,
      13,    0,  109,    2, 0x08,   12 /* Private */,
      14,    0,  110,    2, 0x08,   13 /* Private */,
      15,    0,  111,    2, 0x08,   14 /* Private */,
      16,    0,  112,    2, 0x08,   15 /* Private */,
      17,    0,  113,    2, 0x08,   16 /* Private */,
      18,    0,  114,    2, 0x08,   17 /* Private */,
      19,    0,  115,    2, 0x08,   18 /* Private */,
      20,    0,  116,    2, 0x08,   19 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,    4,
    QMetaType::Void, QMetaType::QJsonArray,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,    8,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RulesEngineView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RulesEngineView.offsetsAndSizes,
    qt_meta_data_RulesEngineView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RulesEngineView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RulesEngineView, std::true_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStreamsReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onRulesReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonArray &, std::false_type>,
        // method 'onRuleStatusReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onOperationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAddStream'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteStream'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddRule'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteRule'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartRule'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStopRule'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onViewSql'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onViewMetrics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RulesEngineView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RulesEngineView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->onStreamsReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 2: _t->onRulesReceived((*reinterpret_cast< std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 3: _t->onRuleStatusReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 4: _t->onOperationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->onAddStream(); break;
        case 6: _t->onDeleteStream(); break;
        case 7: _t->onAddRule(); break;
        case 8: _t->onDeleteRule(); break;
        case 9: _t->onStartRule(); break;
        case 10: _t->onStopRule(); break;
        case 11: _t->onViewSql(); break;
        case 12: _t->onViewMetrics(); break;
        default: ;
        }
    }
}

const QMetaObject *RulesEngineView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RulesEngineView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RulesEngineView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RulesEngineView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
