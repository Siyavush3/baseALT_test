// src/gui/mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QFormLayout> 
#include <QGroupBox>  
#include <QHeaderView> 
#include <QFileDialog> 
#include <QMessageBox> 
#include <QJsonDocument> 
#include <QJsonObject>   
#include <QJsonArray>   
#include <QFile>       
#include <QTextStream> 
#include <QStandardPaths> 


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUi();
    connectSignalsSlots();
    setWindowTitle("ALT Linux RDB Package Comparison");
    setFixedSize(900, 700); 

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240)); 
    palette.setColor(QPalette::WindowText, QColor(30, 30, 30)); 
    palette.setColor(QPalette::Base, QColor(255, 255, 255)); 
    palette.setColor(QPalette::Text, QColor(0, 0, 0)); 
    palette.setColor(QPalette::Button, QColor(200, 200, 200)); 
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0)); 
    setPalette(palette);
}

MainWindow::~MainWindow() {

}

