#include "comparisonworker.h"
#include <stdexcept> // Для std::runtime_error
#include <string>    // Для std::string

// Важно: rdbcompare_init и rdbcompare_cleanup должны быть вызваны
// тем потоком, который использует libcurl.
// Поскольку ComparisonWorker работает в отдельном потоке, он будет отвечать за это.

ComparisonWorker::ComparisonWorker(const QString& branch1, const QString& branch2, QObject *parent)
    : QObject(parent), m_branch1(branch1), m_branch2(branch2), m_cancelRequested(false) {
}

ComparisonWorker::~ComparisonWorker() {
    // Деструктор
}

void ComparisonWorker::doComparisonWork() {
    emit workStarted(); // Сообщаем GUI, что работа началась
    emit workProgress("Инициализация библиотеки и подготовка...");

    // Вызываем глобальную инициализацию вашей C++ библиотеки
    // Это важно, так как curl_global_init() должен быть вызван один раз на поток
    rdbcompare_init(); 

    char* branch1_data_ptr = nullptr;
    char* branch2_data_ptr = nullptr;
    char* comparison_result_ptr = nullptr;

    try {
        if (m_cancelRequested) { // Проверка отмены перед началом
            emit comparisonCancelled();
            return;
        }

        // 1. Получаем данные для первой ветки
        emit workProgress(QString("Запрос пакетов для ветки '%1'...").arg(m_branch1));
        branch1_data_ptr = fetch_package_list(m_branch1.toStdString().c_str());
        if (!branch1_data_ptr) {
            throw std::runtime_error("Не удалось получить данные для ветки " + m_branch1.toStdString());
        }

        if (m_cancelRequested) {
            emit comparisonCancelled();
            // Освобождаем память, выделенную до отмены
            if (branch1_data_ptr) free(branch1_data_ptr);
            return;
        }

        // 2. Получаем данные для второй ветки
        emit workProgress(QString("Запрос пакетов для ветки '%1'...").arg(m_branch2));
        branch2_data_ptr = fetch_package_list(m_branch2.toStdString().c_str());
        if (!branch2_data_ptr) {
            throw std::runtime_error("Не удалось получить данные для ветки " + m_branch2.toStdString());
        }

        if (m_cancelRequested) {
            emit comparisonCancelled();
            // Освобождаем память, выделенную до отмены
            if (branch1_data_ptr) free(branch1_data_ptr);
            if (branch2_data_ptr) free(branch2_data_ptr);
            return;
        }

        // 3. Сравниваем данные
        emit workProgress("Выполнение сравнения пакетов...");
        comparison_result_ptr = compare_packages(branch1_data_ptr, branch2_data_ptr);
        if (!comparison_result_ptr) {
            throw std::runtime_error("Не удалось выполнить сравнение пакетов.");
        }

        emit workProgress("Сравнение завершено. Подготовка результатов...");
        emit comparisonFinished(QString::fromLocal8Bit(comparison_result_ptr)); // Передаем результат
        
    } catch (const std::exception& e) {
        emit comparisonError(QString("Ошибка во время сравнения: %1").arg(e.what()));
    } catch (...) {
        emit comparisonError("Неизвестная ошибка во время сравнения.");
    }

    // Освобождаем память, выделенную C-кодом, в любом случае (даже при ошибке)
    if (branch1_data_ptr) free(branch1_data_ptr);
    if (branch2_data_ptr) free(branch2_data_ptr);
    if (comparison_result_ptr) free(comparison_result_ptr);

    // Вызываем глобальную очистку вашей C++ библиотеки
    rdbcompare_cleanup(); 
}

void ComparisonWorker::cancelRequested() {
    m_cancelRequested = true; // Устанавливаем флаг отмены
    // В зависимости от реализации libcurl, это может не сразу прервать сетевой запрос.
    // Для полной отмены сетевых запросов curl_easy_setopt с CURLOPT_PROGRESSFUNCTION
    // и возвратом ненулевого значения был бы нужен, но это сильно усложнит задачу.
    // Для этого ТЗ достаточно проверки флага между стадиями.
}
