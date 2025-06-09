// src/gui/mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QThread> 
#include "rdbcompare.hpp"
#include "comparisonworker.h" 

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCompareButtonClicked();
    void onCancelButtonClicked();
    void onSaveJsonButtonClicked();
    void onFilterTextChanged(const QString &text);
    void onArchitectureSelected(const QString &arch);


    void onComparisonFinished(const QString& resultJson);
    void onComparisonError(const QString& errorMessage);
    void onComparisonCancelled();
    void onWorkStarted();
    void onWorkProgress(const QString& message);

private:
    QLabel *titleLabel;
    QLineEdit *branch1Input;
    QLineEdit *branch2Input;
    QPushButton *compareButton;

    QLabel *branch1OnlyCountLabel;
    QLabel *branch2OnlyCountLabel;
    QLabel *branch1NewerCountLabel;

    QLineEdit *filterInput;
    QComboBox *archFilterComboBox;
    QTableWidget *resultsTable;

    QLabel *errorLabel;
    QPushButton *cancelButton;
    QPushButton *saveJsonButton;

    QThread *workerThread;
    ComparisonWorker *comparisonWorker;

    void setupUi();
    void connectSignalsSlots();
    void updateCountsDisplay(const std::string& json_summary);
    void populateTable(const std::string& json_architectures);
    void applyTableFilters();
    void displayError(const std::string& errorMessage, bool isError = true);

    std::string fullComparisonResultJson; 
};

#endif // MAINWINDOW_H