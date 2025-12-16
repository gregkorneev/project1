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

static std::string factor_label_short(int idx) {
    return "F" + std::to_string(idx + 1);
}

static std::string factor_label_long(int idx) {
    // Пояснения к факторам (можно менять, если по смыслу уточните после интерпретации)
    switch (idx) {
        case 0: return "Social";
        case 1: return "Achievement";
        case 2: return "FamilyLeisure";
        case 3: return "AgeAbsence";
        case 4: return "Study";
        case 5: return "Other";
        default: return "";
    }
}

static std::string factor_label_full(int idx) {
    return factor_label_short(idx) + "(" + factor_label_long(idx) + ")";
}

static void print_loadings_table(
    const std::vector<std::string>& features,
    const std::vector<std::vector<double>>& L
) {
    const int rows = (int)features.size();
    const int cols = rows > 0 ? (int)L[0].size() : 0;

    const int w_feat = 14;   // ширина колонки "Признак"
    const int w_num  = 12;   // ширина чисел
    const int w_head = 12;   // ширина заголовков факторов

    std::cout << "\nМатрица факторных нагрузок (признаки x факторы):\n";
    std::cout << std::fixed << std::setprecision(3);

    // Заголовок 1: F1..Fk
    std::cout << std::setw(w_feat) << "Признак";
    for (int j = 0; j < cols; ++j) {
        std::cout << std::setw(w_head) << factor_label_short(j);
    }
    std::cout << "\n";

    // Заголовок 2: пояснения факторов
    std::cout << std::setw(w_feat) << "";
    for (int j = 0; j < cols; ++j) {
        std::string s = factor_label_long(j);
        if ((int)s.size() > w_head - 1) s = s.substr(0, w_head - 1); // чтобы не ломало таблицу
        std::cout << std::setw(w_head) << s;
    }
    std::cout << "\n";

    // Разделитель
    std::cout << std::setw(w_feat) << "--------";
    for (int j = 0; j < cols; ++j) std::cout << std::setw(w_head) << "----------";
    std::cout << "\n";

    // Строки данных
    for (int i = 0; i < rows; ++i) {
        std::cout << std::setw(w_feat) << features[i];
        for (int j = 0; j < cols; ++j) {
            std::cout << std::setw(w_num) << L[i][j];
        }
        std::cout << "\n";
    }

    std::cout << "\nПодсказка: обычно |нагрузка| >= 0.4 считается заметной.\n";
}

static void print_top_factor_per_feature(
    const std::vector<std::string>& features,
    const std::vector<std::vector<double>>& L
) {
    const int rows = (int)features.size();
    const int cols = rows > 0 ? (int)L[0].size() : 0;

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
                  << factor_label_full(best_j) << " = " << best_val << "\n";
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

    const std::string load_path = "results/factor_loadings.csv";
    std::vector<std::string> factor_cols;
    for (int j = 0; j < k; ++j) factor_cols.push_back(factor_label_short(j)); // в CSV оставляем коротко F1..F6

    if (!write_table_csv(load_path, features, factor_cols, L)) {
        std::cerr << "Не удалось сохранить файл: " << load_path << "\n";
        return 1;
    }
    std::cout << "Файл " << load_path << " сохранён.\n";
    saved_files.push_back(load_path);

    // Наглядный вывод
    print_loadings_table(features, L);
    print_top_factor_per_feature(features, L);

    // ===== Итоговый список файлов =====
    std::cout << "\nСохранённые файлы в папке results:\n";
    for (const auto& p : saved_files) {
        std::cout << "- " << p << "\n";
    }

    return 0;
}
