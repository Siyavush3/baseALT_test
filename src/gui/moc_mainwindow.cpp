/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[17];
    char stringdata0[256];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 22), // "onCompareButtonClicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "onCancelButtonClicked"
QT_MOC_LITERAL(4, 57, 23), // "onSaveJsonButtonClicked"
QT_MOC_LITERAL(5, 81, 19), // "onFilterTextChanged"
QT_MOC_LITERAL(6, 101, 4), // "text"
QT_MOC_LITERAL(7, 106, 22), // "onArchitectureSelected"
QT_MOC_LITERAL(8, 129, 4), // "arch"
QT_MOC_LITERAL(9, 134, 20), // "onComparisonFinished"
QT_MOC_LITERAL(10, 155, 10), // "resultJson"
QT_MOC_LITERAL(11, 166, 17), // "onComparisonError"
QT_MOC_LITERAL(12, 184, 12), // "errorMessage"
QT_MOC_LITERAL(13, 197, 21), // "onComparisonCancelled"
QT_MOC_LITERAL(14, 219, 13), // "onWorkStarted"
QT_MOC_LITERAL(15, 233, 14), // "onWorkProgress"
QT_MOC_LITERAL(16, 248, 7) // "message"

    },
    "MainWindow\0onCompareButtonClicked\0\0"
    "onCancelButtonClicked\0onSaveJsonButtonClicked\0"
    "onFilterTextChanged\0text\0"
    "onArchitectureSelected\0arch\0"
    "onComparisonFinished\0resultJson\0"
    "onComparisonError\0errorMessage\0"
    "onComparisonCancelled\0onWorkStarted\0"
    "onWorkProgress\0message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    1,   67,    2, 0x08 /* Private */,
       7,    1,   70,    2, 0x08 /* Private */,
       9,    1,   73,    2, 0x08 /* Private */,
      11,    1,   76,    2, 0x08 /* Private */,
      13,    0,   79,    2, 0x08 /* Private */,
      14,    0,   80,    2, 0x08 /* Private */,
      15,    1,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCompareButtonClicked(); break;
        case 1: _t->onCancelButtonClicked(); break;
        case 2: _t->onSaveJsonButtonClicked(); break;
        case 3: _t->onFilterTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onArchitectureSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onComparisonFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onComparisonError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onComparisonCancelled(); break;
        case 8: _t->onWorkStarted(); break;
        case 9: _t->onWorkProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
