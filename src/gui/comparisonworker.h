#ifndef COMPARISONWORKER_H
#define COMPARISONWORKER_H

#include <QObject>
#include <QString>
#include <iostream> 
#include "rdbcompare.hpp" 

class ComparisonWorker : public QObject {
    Q_OBJECT 

public:
    explicit ComparisonWorker(const QString& branch1, const QString& branch2, QObject *parent = nullptr);
    ~ComparisonWorker();

public slots:
    void doComparisonWork(); 
    void cancelRequested();  

signals:
    void comparisonFinished(const QString& resultJson); 
    void comparisonError(const QString& errorMessage); 
    void comparisonCancelled(); 
    void workStarted(); 
    void workProgress(const QString& message); 

private:
    QString m_branch1;
    QString m_branch2;
    bool m_cancelRequested; 
};

#endif // COMPARISONWORKER_H
