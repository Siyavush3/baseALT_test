/****************************************************************************
** Meta object code from reading C++ file 'comparisonworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "comparisonworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'comparisonworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ComparisonWorker_t {
    QByteArrayData data[12];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ComparisonWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ComparisonWorker_t qt_meta_stringdata_ComparisonWorker = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ComparisonWorker"
QT_MOC_LITERAL(1, 17, 18), // "comparisonFinished"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 10), // "resultJson"
QT_MOC_LITERAL(4, 48, 15), // "comparisonError"
QT_MOC_LITERAL(5, 64, 12), // "errorMessage"
QT_MOC_LITERAL(6, 77, 19), // "comparisonCancelled"
QT_MOC_LITERAL(7, 97, 11), // "workStarted"
QT_MOC_LITERAL(8, 109, 12), // "workProgress"
QT_MOC_LITERAL(9, 122, 7), // "message"
QT_MOC_LITERAL(10, 130, 16), // "doComparisonWork"
QT_MOC_LITERAL(11, 147, 15) // "cancelRequested"

    },
    "ComparisonWorker\0comparisonFinished\0"
    "\0resultJson\0comparisonError\0errorMessage\0"
    "comparisonCancelled\0workStarted\0"
    "workProgress\0message\0doComparisonWork\0"
    "cancelRequested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ComparisonWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       6,    0,   55,    2, 0x06 /* Public */,
       7,    0,   56,    2, 0x06 /* Public */,
       8,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   60,    2, 0x0a /* Public */,
      11,    0,   61,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ComparisonWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ComparisonWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->comparisonFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->comparisonError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->comparisonCancelled(); break;
        case 3: _t->workStarted(); break;
        case 4: _t->workProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->doComparisonWork(); break;
        case 6: _t->cancelRequested(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ComparisonWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ComparisonWorker::comparisonFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ComparisonWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ComparisonWorker::comparisonError)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ComparisonWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ComparisonWorker::comparisonCancelled)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ComparisonWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ComparisonWorker::workStarted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ComparisonWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ComparisonWorker::workProgress)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ComparisonWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ComparisonWorker.data,
    qt_meta_data_ComparisonWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ComparisonWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ComparisonWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ComparisonWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ComparisonWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ComparisonWorker::comparisonFinished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ComparisonWorker::comparisonError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ComparisonWorker::comparisonCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ComparisonWorker::workStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ComparisonWorker::workProgress(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
