// src/gui/main.cpp
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

    rdbcompare_init(); 

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int result = a.exec();

    rdbcompare_cleanup();

    return result;
}
