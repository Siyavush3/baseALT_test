# src/gui/alt_rdb_gui_app.pro
QT       += widgets # Используем QtWidgets для классического GUI

TARGET   = alt_rdb_gui_app

TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

LIBS += -lrdbcompare


