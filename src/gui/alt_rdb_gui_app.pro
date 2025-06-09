# src/gui/alt_rdb_gui_app.pro
QT       += widgets

TARGET   = alt_rdb_gui_app
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           comparisonworker.cpp 

HEADERS += mainwindow.h \
           comparisonworker.h  
INCLUDEPATH += $$PWD/../lib

LIBS += -lrdbcompare