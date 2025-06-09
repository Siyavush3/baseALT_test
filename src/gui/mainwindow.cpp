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
    : QMainWindow(parent), workerThread(nullptr), comparisonWorker(nullptr) { // Инициализируем указатели
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

    cancelButton->setEnabled(false);
    saveJsonButton->setEnabled(false);
}

MainWindow::~MainWindow() {
    if (workerThread && workerThread->isRunning()) {
        comparisonWorker->cancelRequested();
        workerThread->quit();
        workerThread->wait(3000);
    }
}

void MainWindow::setupUi() {

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    titleLabel = new QLabel("ALT Linux RDB Package Comparison Tool", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont font = titleLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    QGroupBox *inputGroup = new QGroupBox("Сравнение веток", this);
    QFormLayout *inputLayout = new QFormLayout(inputGroup);
    branch1Input = new QLineEdit(this);
    branch1Input->setPlaceholderText("Например: sisyphus");
    branch1Input->setText("sisyphus");
    branch2Input = new QLineEdit(this);
    branch2Input->setPlaceholderText("Например: p10");
    branch2Input->setText("p10");
    inputLayout->addRow("Ветка 1:", branch1Input);
    inputLayout->addRow("Ветка 2:", branch2Input);

    compareButton = new QPushButton("Начать сравнение", this);
    inputLayout->addRow(compareButton);

    mainLayout->addWidget(inputGroup);

    QGroupBox *countsGroup = new QGroupBox("Сводка различий", this);
    QHBoxLayout *countsLayout = new QHBoxLayout(countsGroup);
    branch1OnlyCountLabel = new QLabel("Только в Ветке 1: 0", this);
    branch2OnlyCountLabel = new QLabel("Только в Ветке 2: 0", this);
    branch1NewerCountLabel = new QLabel("Новее в Ветке 1: 0", this);
    countsLayout->addWidget(branch1OnlyCountLabel);
    countsLayout->addWidget(branch2OnlyCountLabel);
    countsLayout->addWidget(branch1NewerCountLabel);
    mainLayout->addWidget(countsGroup);

    QGroupBox *filterGroup = new QGroupBox("Фильтры", this);
    QHBoxLayout *filterLayout = new QHBoxLayout(filterGroup);
    filterInput = new QLineEdit(this);
    filterInput->setPlaceholderText("Фильтр по имени пакета...");
    filterLayout->addWidget(new QLabel("Имя пакета:", this));
    filterLayout->addWidget(filterInput);

    archFilterComboBox = new QComboBox(this);
    archFilterComboBox->addItem("Все архитектуры");
    filterLayout->addWidget(new QLabel("Архитектура:", this));
    filterLayout->addWidget(archFilterComboBox);
    mainLayout->addWidget(filterGroup);

    resultsTable = new QTableWidget(this);
    resultsTable->setColumnCount(8);
    resultsTable->setHorizontalHeaderLabels({"Архитектура", "Имя пакета", "Эпоха", "Версия (B1)", "Релиз (B1)", "Версия (B2)", "Релиз (B2)", "Категория"});
    resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsTable->setSortingEnabled(true);
    mainLayout->addWidget(resultsTable);

    errorLabel = new QLabel(" ", this);
    errorLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(errorLabel);

    QHBoxLayout *bottomButtonsLayout = new QHBoxLayout();
    bottomButtonsLayout->addStretch(1);
    cancelButton = new QPushButton("Отмена", this);
    saveJsonButton = new QPushButton("Сохранить в JSON", this);
    bottomButtonsLayout->addWidget(cancelButton);
    bottomButtonsLayout->addWidget(saveJsonButton);
    mainLayout->addLayout(bottomButtonsLayout);
}

void MainWindow::connectSignalsSlots() {
    connect(compareButton, &QPushButton::clicked, this, &MainWindow::onCompareButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);
    connect(saveJsonButton, &QPushButton::clicked, this, &MainWindow::onSaveJsonButtonClicked);
    
    connect(filterInput, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
    connect(archFilterComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &MainWindow::onArchitectureSelected);
}

// --- СЛОТЫ ДЛЯ ОБРАБОТКИ КНОПОК И ПОТОКА ---
void MainWindow::onCompareButtonClicked() {
    resultsTable->clearContents();
    resultsTable->setRowCount(0);
    archFilterComboBox->clear();
    archFilterComboBox->addItem("Все архитектуры");
    branch1OnlyCountLabel->setText("Только в Ветке 1: 0");
    branch2OnlyCountLabel->setText("Только в Ветке 2: 0");
    branch1NewerCountLabel->setText("Новее в Ветке 1: 0");
    errorLabel->setText(" ");
    fullComparisonResultJson.clear();

    QString branch1 = branch1Input->text();
    QString branch2 = branch2Input->text();

    if (branch1.isEmpty() || branch2.isEmpty()) {
        displayError("Пожалуйста, введите оба имени ветки.");
        return;
    }

    compareButton->setEnabled(false);
    branch1Input->setEnabled(false);
    branch2Input->setEnabled(false);
    cancelButton->setEnabled(true);
    saveJsonButton->setEnabled(false);

    displayError("Начало загрузки и сравнения данных...", false);

    workerThread = new QThread(this);
    comparisonWorker = new ComparisonWorker(branch1, branch2);
    comparisonWorker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, comparisonWorker, &ComparisonWorker::doComparisonWork);

    connect(comparisonWorker, &ComparisonWorker::comparisonFinished, this, &MainWindow::onComparisonFinished);
    connect(comparisonWorker, &ComparisonWorker::comparisonError, this, &MainWindow::onComparisonError);
    connect(comparisonWorker, &ComparisonWorker::comparisonCancelled, this, &MainWindow::onComparisonCancelled);
    connect(comparisonWorker, &ComparisonWorker::workStarted, this, &MainWindow::onWorkStarted);
    connect(comparisonWorker, &ComparisonWorker::workProgress, this, &MainWindow::onWorkProgress);

    connect(workerThread, &QThread::finished, comparisonWorker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    connect(cancelButton, &QPushButton::clicked, comparisonWorker, &ComparisonWorker::cancelRequested);

    workerThread->start();
}

void MainWindow::onComparisonFinished(const QString& resultJson) {
    if (workerThread) {
        workerThread->quit();
    }

    fullComparisonResultJson = resultJson.toStdString();

    QJsonDocument doc = QJsonDocument::fromJson(resultJson.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        displayError("Ошибка: Некорректный формат JSON-ответа от библиотеки.", true);
        return;
    }

    QJsonObject rootObj = doc.object();
    
    QJsonObject summaryObj = rootObj.value("summary").toObject();
    updateCountsDisplay(QJsonDocument(summaryObj).toJson(QJsonDocument::Compact).toStdString());

    QJsonObject architecturesObj = rootObj.value("architectures").toObject();
    populateTable(QJsonDocument(architecturesObj).toJson(QJsonDocument::Compact).toStdString());

    displayError("Сравнение успешно завершено.", false); 
    compareButton->setEnabled(true);
    branch1Input->setEnabled(true);
    branch2Input->setEnabled(true);
    cancelButton->setEnabled(false);
    saveJsonButton->setEnabled(true); 
}

void MainWindow::onComparisonError(const QString& errorMessage) {
    if (workerThread) {
        workerThread->quit();
    }

    displayError(errorMessage.toStdString(), true); 
    compareButton->setEnabled(true);
    branch1Input->setEnabled(true);
    branch2Input->setEnabled(true);
    cancelButton->setEnabled(false);
    saveJsonButton->setEnabled(false);
}

void MainWindow::onComparisonCancelled() {
    if (workerThread) {
        workerThread->quit();
    }
    
    resultsTable->clearContents();
    resultsTable->setRowCount(0);
    branch1OnlyCountLabel->setText("Только в Ветке 1: 0");
    branch2OnlyCountLabel->setText("Только в Ветке 2: 0");
    branch1NewerCountLabel->setText("Новее в Ветке 1: 0");
    errorLabel->setText("Операция отменена.");
    fullComparisonResultJson.clear();
    
    compareButton->setEnabled(true);
    branch1Input->setEnabled(true);
    branch2Input->setEnabled(true);
    cancelButton->setEnabled(false);
    saveJsonButton->setEnabled(false);
}
void MainWindow::onWorkStarted() {

}

void MainWindow::onWorkProgress(const QString& message) {
    displayError(message.toStdString(), false);
}

void MainWindow::onCancelButtonClicked() {

    cancelButton->setEnabled(false); 
    displayError("Запрашивается отмена...", false);
}

void MainWindow::onSaveJsonButtonClicked() {
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить результаты сравнения",
                                                   defaultDir + "/comparison_results.json",
                                                   "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    // Создаем JSON-документ из полного результата
    QJsonDocument fullDoc = QJsonDocument::fromJson(QString::fromStdString(fullComparisonResultJson).toUtf8());
    if (fullDoc.isNull() || !fullDoc.isObject()) {
        displayError("Ошибка: Некорректные данные для сохранения.", true);
        return;
    }

    QJsonObject filteredRootObj;
    QJsonObject filteredArchitecturesObj;
    QJsonObject originalArchitecturesObj = fullDoc.object().value("architectures").toObject();

    QString filterText = filterInput->text().toLower();
    QString selectedArch = archFilterComboBox->currentText();
    bool filterByArch = (selectedArch != "Все архитектуры");

    // Проходим по исходным данным и применяем те же фильтры
    for (auto it = originalArchitecturesObj.begin(); it != originalArchitecturesObj.end(); ++it) {
        QString archName = it.key();
        QJsonObject archData = it.value().toObject();

        if (filterByArch && archName != selectedArch) {
            continue; // Пропускаем, если не соответствует фильтру по архитектуре
        }

        QJsonObject filteredArchData;
        QStringList categories = {"branch1_only", "branch2_only", "branch1_newer"};
        
        bool archHasMatchingPackages = false; // Флаг, чтобы знать, есть ли что-то в этой архитектуре после фильтров

        for (const QString& category : categories) {
            QJsonObject originalCategoryObj = archData.value(category).toObject();
            QJsonArray originalPackagesArray = originalCategoryObj.value("packages").toArray();
            
            QJsonArray filteredPackagesArray;
            int filteredCount = 0;

            for (const QJsonValue &pkgValue : originalPackagesArray) {
                // Извлекаем имя пакета для текстового фильтра
                QString pkgNameForFilter;
                if (category == "branch1_only" || category == "branch2_only") {
                    pkgNameForFilter = pkgValue.toString();
                } else if (category == "branch1_newer") {
                    pkgNameForFilter = pkgValue.toObject().value("name").toString();
                }


                if (!filterText.isEmpty() && !pkgNameForFilter.toLower().contains(filterText)) {
                    continue; 
                }

                filteredPackagesArray.append(pkgValue); // Добавляем пакет, если он прошел фильтры
                filteredCount++;
            }

            if (filteredCount > 0) {
                QJsonObject filteredCategoryObj;
                filteredCategoryObj.insert("count", filteredCount);
                filteredCategoryObj.insert("packages", filteredPackagesArray);
                filteredArchData.insert(category, filteredCategoryObj);
                archHasMatchingPackages = true;
            }
        }
        
        if (archHasMatchingPackages) {
            filteredArchitecturesObj.insert(archName, filteredArchData);
        }
    }

    filteredRootObj.insert("architectures", filteredArchitecturesObj);


    filteredRootObj.insert("summary", fullDoc.object().value("summary"));

    QJsonDocument finalDoc(filteredRootObj);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << finalDoc.toJson(QJsonDocument::Indented); 
        file.close();
        displayError("Отфильтрованные результаты успешно сохранены в " + fileName.toStdString() + ".", false);
    } else {
        displayError("Не удалось открыть файл для сохранения: " + file.errorString().toStdString(), true);
    }
}

void MainWindow::onFilterTextChanged(const QString &text) {
    applyTableFilters();
}

void MainWindow::onArchitectureSelected(const QString &arch) {
    applyTableFilters();
}

void MainWindow::updateCountsDisplay(const std::string& json_summary_str) {
    if (json_summary_str.empty()) {
        branch1OnlyCountLabel->setText("Только в Ветке 1: 0");
        branch2OnlyCountLabel->setText("Только в Ветке 2: 0");
        branch1NewerCountLabel->setText("Новее в Ветке 1: 0");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(json_summary_str).toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject summary = doc.object();
        branch1OnlyCountLabel->setText(QString("Только в Ветке 1: %1").arg(summary.value("total_branch1_only_count").toInt()));
        branch2OnlyCountLabel->setText(QString("Только в Ветке 2: %1").arg(summary.value("total_branch2_only_count").toInt()));
        branch1NewerCountLabel->setText(QString("Новее в Ветке 1: %1").arg(summary.value("total_branch1_newer_count").toInt()));
    } else {
        branch1OnlyCountLabel->setText("Только в Ветке 1: N/A");
        branch2OnlyCountLabel->setText("Только в Ветке 2: N/A");
        branch1NewerCountLabel->setText("Новее в Ветке 1: N/A");
    }
}

void MainWindow::populateTable(const std::string& json_architectures_str) {
    resultsTable->clearContents();
    resultsTable->setRowCount(0);
    archFilterComboBox->clear();
    archFilterComboBox->addItem("Все архитектуры"); // Добавляем по умолчанию

    if (json_architectures_str.empty()) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(json_architectures_str).toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        displayError("Некорректный формат данных для заполнения таблицы.", true);
        return;
    }

    QJsonObject architectures = doc.object();
    int currentRow = 0;
    QSet<QString> addedArchitectures; // Для уникальных архитектур в комбобоксе

    // Заполняем ComboBox архитектурами
    for (auto it = architectures.begin(); it != architectures.end(); ++it) {
        QString archName = it.key();
        if (!addedArchitectures.contains(archName)) {
            archFilterComboBox->addItem(archName);
            addedArchitectures.insert(archName);
        }
    }

    // Теперь заполняем таблицу, проходя по тем же данным
    for (auto it = architectures.begin(); it != architectures.end(); ++it) {
        QString archName = it.key();
        QJsonObject archData = it.value().toObject();

        QStringList categories = {"branch1_only", "branch2_only", "branch1_newer"};
        for (const QString& category : categories) {
            QJsonObject categoryObj = archData.value(category).toObject();
            QJsonArray packagesArray = categoryObj.value("packages").toArray();

            for (const QJsonValue &pkgValue : packagesArray) {
                QString name, epoch, ver1, rel1, ver2, rel2, categoryText;
                categoryText = category; 
                categoryText.replace("_", " ");
                if (!categoryText.isEmpty()) {
                    categoryText[0] = categoryText[0].toUpper(); // Делаем первый символ заглавным
                }

                if (category == "branch1_only") {
                    name = pkgValue.toString();
                    ver1 = "Присутствует"; rel1 = "Присутствует";
                    ver2 = "Н/Д"; rel2 = "Н/Д";
                    epoch = "Н/Д"; // Эпоха для "only" не передаётся в API
                } else if (category == "branch2_only") {
                    name = pkgValue.toString();
                    ver1 = "Н/Д"; rel1 = "Н/Д";
                    ver2 = "Присутствует"; rel2 = "Присутствует";
                    epoch = "Н/Д";
                } else if (category == "branch1_newer") {
                    QJsonObject pkgObj = pkgValue.toObject(); 
                    name = pkgObj.value("name").toString();
                    
                    // Разделяем branch1_version_release на версию и релиз
                    QString b1_ver_rel = pkgObj.value("branch1_version_release").toString();
                    int lastDashB1 = b1_ver_rel.lastIndexOf('-');
                    if (lastDashB1 != -1) {
                        ver1 = b1_ver_rel.left(lastDashB1);
                        rel1 = b1_ver_rel.mid(lastDashB1 + 1);
                    } else {
                        ver1 = b1_ver_rel; rel1 = "N/A";
                    }

                    // Разделяем branch2_version_release на версию и релиз
                    QString b2_ver_rel = pkgObj.value("branch2_version_release").toString();
                    int lastDashB2 = b2_ver_rel.lastIndexOf('-');
                    if (lastDashB2 != -1) {
                        ver2 = b2_ver_rel.left(lastDashB2);
                        rel2 = b2_ver_rel.mid(lastDashB2 + 1);
                    } else {
                        ver2 = b2_ver_rel; rel2 = "N/A";
                    }
                    epoch = "Н/Д"; // Эпоха не передаётся в этом формате
                }
                
                resultsTable->setRowCount(currentRow + 1);
                resultsTable->setItem(currentRow, 0, new QTableWidgetItem(archName));
                resultsTable->setItem(currentRow, 1, new QTableWidgetItem(name));
                resultsTable->setItem(currentRow, 2, new QTableWidgetItem(epoch));
                resultsTable->setItem(currentRow, 3, new QTableWidgetItem(ver1));
                resultsTable->setItem(currentRow, 4, new QTableWidgetItem(rel1));
                resultsTable->setItem(currentRow, 5, new QTableWidgetItem(ver2));
                resultsTable->setItem(currentRow, 6, new QTableWidgetItem(rel2));
                resultsTable->setItem(currentRow, 7, new QTableWidgetItem(categoryText));
                
                currentRow++;
            }
        }
    }
    resultsTable->resizeColumnsToContents();
}

void MainWindow::applyTableFilters() {
    QString filterText = filterInput->text().toLower();
    QString selectedArch = archFilterComboBox->currentText();
    bool filterByArch = (selectedArch != "Все архитектуры");

    for (int row = 0; row < resultsTable->rowCount(); ++row) {
        bool hideRow = false;


        if (!filterText.isEmpty()) {
            bool rowMatchesFilter = false;

            for (int col : {1, 7}) { // Столбец 1: Имя пакета, Столбец 7: Категория
                QTableWidgetItem *item = resultsTable->item(row, col);
                if (item && item->text().toLower().contains(filterText)) {
                    rowMatchesFilter = true;
                    break;
                }
            }
            if (!rowMatchesFilter) {
                hideRow = true;
            }
        }


        if (!hideRow && filterByArch) {
            QTableWidgetItem *archItem = resultsTable->item(row, 0); 
            if (archItem && archItem->text() != selectedArch) {
                hideRow = true;
            }
        }
        resultsTable->setRowHidden(row, hideRow);
    }
}

void MainWindow::displayError(const std::string& message, bool isError) {
    errorLabel->setText(QString::fromStdString(message));
    if (isError) {
        errorLabel->setStyleSheet("color: red;");
    } else {
        errorLabel->setStyleSheet("color: black;"); // Используем черный для статусных сообщений
    }
}