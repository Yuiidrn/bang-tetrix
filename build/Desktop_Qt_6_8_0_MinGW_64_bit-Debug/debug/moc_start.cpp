/****************************************************************************
** Meta object code from reading C++ file 'start.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../start.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'start.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSStartENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSStartENDCLASS = QtMocHelpers::stringData(
    "Start",
    "scoreRecord",
    "",
    "end_score",
    "end_combo",
    "showGameOverDialog",
    "switchToGame",
    "on_showScoreTableButton_clicked",
    "onScoreTableTypeChanged",
    "ScoreTableType",
    "newType",
    "onScoreTableClosed",
    "onWorldRankingsUpdated",
    "onNetworkError",
    "errorMessage",
    "onSyncCompleted",
    "success",
    "onScoreUploadCompleted"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSStartENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   74,    2, 0x0a,    1 /* Public */,
       5,    0,   79,    2, 0x0a,    4 /* Public */,
       6,    0,   80,    2, 0x08,    5 /* Private */,
       7,    0,   81,    2, 0x08,    6 /* Private */,
       8,    1,   82,    2, 0x08,    7 /* Private */,
      11,    0,   85,    2, 0x08,    9 /* Private */,
      12,    0,   86,    2, 0x08,   10 /* Private */,
      13,    1,   87,    2, 0x08,   11 /* Private */,
      15,    1,   90,    2, 0x08,   13 /* Private */,
      17,    1,   93,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,

       0        // eod
};

Q_CONSTINIT const QMetaObject Start::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSStartENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSStartENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSStartENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Start, std::true_type>,
        // method 'scoreRecord'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'showGameOverDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'switchToGame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_showScoreTableButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onScoreTableTypeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ScoreTableType, std::false_type>,
        // method 'onScoreTableClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWorldRankingsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onNetworkError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onSyncCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onScoreUploadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void Start::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Start *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->scoreRecord((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->showGameOverDialog(); break;
        case 2: _t->switchToGame(); break;
        case 3: _t->on_showScoreTableButton_clicked(); break;
        case 4: _t->onScoreTableTypeChanged((*reinterpret_cast< std::add_pointer_t<ScoreTableType>>(_a[1]))); break;
        case 5: _t->onScoreTableClosed(); break;
        case 6: _t->onWorldRankingsUpdated(); break;
        case 7: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onSyncCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->onScoreUploadCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *Start::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Start::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSStartENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Start::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
