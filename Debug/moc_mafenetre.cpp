/****************************************************************************
** Meta object code from reading C++ file 'mafenetre.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mafenetre.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mafenetre.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_mafenetre_t {
    QByteArrayData data[17];
    char stringdata[202];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mafenetre_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mafenetre_t qt_meta_stringdata_mafenetre = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 14),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 23),
QT_MOC_LITERAL(4, 50, 16),
QT_MOC_LITERAL(5, 67, 11),
QT_MOC_LITERAL(6, 79, 12),
QT_MOC_LITERAL(7, 92, 10),
QT_MOC_LITERAL(8, 103, 10),
QT_MOC_LITERAL(9, 114, 10),
QT_MOC_LITERAL(10, 125, 11),
QT_MOC_LITERAL(11, 137, 14),
QT_MOC_LITERAL(12, 152, 12),
QT_MOC_LITERAL(13, 165, 11),
QT_MOC_LITERAL(14, 177, 13),
QT_MOC_LITERAL(15, 191, 8),
QT_MOC_LITERAL(16, 200, 1)
    },
    "mafenetre\0retournerChoix\0\0"
    "retournerChoixConstante\0selectionFichier\0"
    "std::string\0std::string&\0extensions\0"
    "adresseMNT\0adresseGPX\0activerSpin\0"
    "desactiverSpin\0lancerOpenGl\0activerAide\0"
    "desactiverGpx\0setValue\0a"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mafenetre[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x0a /* Public */,
       4,    1,   71,    2, 0x0a /* Public */,
       8,    0,   74,    2, 0x0a /* Public */,
       9,    0,   75,    2, 0x0a /* Public */,
      10,    0,   76,    2, 0x0a /* Public */,
      11,    0,   77,    2, 0x0a /* Public */,
      12,    0,   78,    2, 0x0a /* Public */,
      13,    0,   79,    2, 0x0a /* Public */,
      14,    0,   80,    2, 0x0a /* Public */,
      15,    1,   81,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 5, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,

       0        // eod
};

void mafenetre::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mafenetre *_t = static_cast<mafenetre *>(_o);
        switch (_id) {
        case 0: _t->retournerChoix(); break;
        case 1: _t->retournerChoixConstante(); break;
        case 2: { std::string _r = _t->selectionFichier((*reinterpret_cast< std::string(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::string*>(_a[0]) = _r; }  break;
        case 3: _t->adresseMNT(); break;
        case 4: _t->adresseGPX(); break;
        case 5: _t->activerSpin(); break;
        case 6: _t->desactiverSpin(); break;
        case 7: _t->lancerOpenGl(); break;
        case 8: _t->activerAide(); break;
        case 9: _t->desactiverGpx(); break;
        case 10: _t->setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject mafenetre::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_mafenetre.data,
      qt_meta_data_mafenetre,  qt_static_metacall, 0, 0}
};


const QMetaObject *mafenetre::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mafenetre::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mafenetre.stringdata))
        return static_cast<void*>(const_cast< mafenetre*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int mafenetre::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
