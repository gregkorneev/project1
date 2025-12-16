#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>

#include "csv_reader.h"
#include "standardize.h"
#include "correlation.h"
#include "csv_writer.h"
#include "eigen_analysis.h"
#include "eigen_results_writer.h"
#include "factor_loadings.h"

int main() {
    std::vector<std::string> features = {
        "age", "studytime", "failures", "famrel", "freetime",
        "goout", "Dalc", "Walc", "absences", "G3"
    };

    // Будем складывать сюда пути всех сохранённых файлов
    std::vector<std::string> saved_files;

    std::vector<std::vector<double>> X;
    std::string err;

    if (!read_students_csv("data/students.csv", features, X, err)) {
        std::cerr << "Ошибка чтения CSV: " << err << "\n";
        return 1;
    }

    std::cout << "Загружено строк: " << X.size() << "\n";
    std::cout << "Число признаков: " << X[0].size() << "\n";

    std::vector<double> mean, stddev;
    if (!standardize_inplace(X, mean, stddev)) {
        std::cerr << "Ошибка стандартизации.\n";
        return 1;
    }
    std::cout << "Стандартизация выполнена.\n";

    // Создаём папку results
    std::filesystem::create_directory("results");

    // ===== Корреляционная матрица =====
    auto R = correlation_matrix(X);

    const std::string corr_path = "results/correlation_matrix.csv";
    if (!write_matrix_csv(corr_path, features, R)) {
        std::cerr << "Не удалось сохранить файл: " << corr_path << "\n";
        return 1;
    }
    std::cout << "Корреляционная матрица сохранена в: " << corr_path << "\n";
    saved_files.push_back(corr_path);

    // ===== Собственные значения =====
    EigenReport rep = analyze_eigenvalues(R);

    const std::string eig_path = "results/eigenvalues.csv";
    if (!write_eigenvalues_csv(eig_path, rep.eigenvalues, rep.explained, rep.cumulative)) {
        std::cerr << "Не удалось сохранить файл: " << eig_path << "\n";
        return 1;
    }
    std::cout << "Файл " << eig_path << " сохранён.\n";
    saved_files.push_back(eig_path);

    int k = rep.k_for_75;
    std::cout << "\nВыбрано факторов: k = " << k << "\n";

    // ===== Факторные нагрузки =====
    auto L = compute_factor_loadings(R, k);

    // Имена факторов (F1..Fk)
    std::vector<std::string> factor_names;
    for (int i = 0; i < k; ++i)
        factor_names.push_back("F" + std::to_string(i + 1));

    const std::string load_path = "results/factor_loadings.csv";
    if (!write_matrix_csv(load_path, factor_names, L)) {
        std::cerr << "Не удалось сохранить файл: " << load_path << "\n";
        return 1;
    }
    std::cout << "Файл " << load_path << " сохранён.\n";
    saved_files.push_back(load_path);

    // Вывод сильных нагрузок
    std::cout << "\nФакторные нагрузки (|значение| >= 0.4):\n";
    std::cout << std::fixed << std::setprecision(3);

    for (size_t i = 0; i < features.size(); ++i) {
        std::cout << features[i] << ": ";
        bool printed = false;

        for (int f = 0; f < k; ++f) {
            double val = L[i][f];
            if (std::abs(val) >= 0.4) {
                std::cout << "F" << (f + 1) << "=" << val << "  ";
                printed = true;
            }
        }

        if (!printed) {
            std::cout << "(нет сильных нагрузок)";
        }
        std::cout << "\n";
    }

    // ===== Итоговый список файлов =====
    std::cout << "\nСохранённые файлы в папке results:\n";
    for (const auto& p : saved_files) {
        std::cout << "- " << p << "\n";
    }

    return 0;
}
