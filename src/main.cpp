#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>
#include <cmath>

#include "csv_reader.h"
#include "standardize.h"
#include "correlation.h"
#include "csv_writer.h"
#include "eigen_analysis.h"
#include "eigen_results_writer.h"
#include "factor_loadings.h"

static void print_loadings_table(
    const std::vector<std::string>& features,
    const std::vector<std::string>& factor_names,
    const std::vector<std::vector<double>>& L
) {
    int rows = (int)features.size();
    int cols = (int)factor_names.size();

    std::cout << "\nМатрица факторных нагрузок (признаки x факторы):\n";
    std::cout << std::fixed << std::setprecision(3);

    // header
    std::cout << std::setw(12) << "Признак";
    for (int j = 0; j < cols; ++j) {
        std::cout << std::setw(8) << factor_names[j];
    }
    std::cout << "\n";

    // rows
    for (int i = 0; i < rows; ++i) {
        std::cout << std::setw(12) << features[i];
        for (int j = 0; j < cols; ++j) {
            std::cout << std::setw(8) << L[i][j];
        }
        std::cout << "\n";
    }

    // подсказка по интерпретации
    std::cout << "\nПодсказка: обычно |нагрузка| >= 0.4 считается заметной.\n";
}

static void print_top_factor_per_feature(
    const std::vector<std::string>& features,
    const std::vector<std::string>& factor_names,
    const std::vector<std::vector<double>>& L
) {
    int rows = (int)features.size();
    int cols = (int)factor_names.size();

    std::cout << "\nГлавный фактор для каждого признака (максимум |нагрузки|):\n";
    std::cout << std::fixed << std::setprecision(3);

    for (int i = 0; i < rows; ++i) {
        int best_j = 0;
        double best_val = L[i][0];

        for (int j = 1; j < cols; ++j) {
            if (std::abs(L[i][j]) > std::abs(best_val)) {
                best_val = L[i][j];
                best_j = j;
            }
        }

        std::cout << "- " << features[i] << " -> "
                  << factor_names[best_j] << " = " << best_val << "\n";
    }
}

int main() {
    std::vector<std::string> features = {
        "age", "studytime", "failures", "famrel", "freetime",
        "goout", "Dalc", "Walc", "absences", "G3"
    };

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

    std::filesystem::create_directory("results");

    // ===== Корреляции =====
    auto R = correlation_matrix(X);

    const std::string corr_path = "results/correlation_matrix.csv";
    if (!write_square_matrix_csv(corr_path, features, R)) {
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

    // ===== Нагрузки =====
    auto L = compute_factor_loadings(R, k);

    std::vector<std::string> factor_names;
    for (int i = 0; i < k; ++i) {
        factor_names.push_back("F" + std::to_string(i + 1));
    }

    const std::string load_path = "results/factor_loadings.csv";
    if (!write_table_csv(load_path, features, factor_names, L)) {
        std::cerr << "Не удалось сохранить файл: " << load_path << "\n";
        return 1;
    }
    std::cout << "Файл " << load_path << " сохранён.\n";
    saved_files.push_back(load_path);

    // Наглядный вывод в терминал
    print_loadings_table(features, factor_names, L);
    print_top_factor_per_feature(features, factor_names, L);

    // ===== Итоговый список файлов =====
    std::cout << "\nСохранённые файлы в папке results:\n";
    for (const auto& p : saved_files) {
        std::cout << "- " << p << "\n";
    }

    return 0;
}
