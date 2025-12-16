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

    auto R = correlation_matrix(X);

    std::filesystem::create_directory("results");

    write_matrix_csv("results/correlation_matrix.csv", features, R);

    EigenReport rep = analyze_eigenvalues(R);

    int k = rep.k_for_75;
    std::cout << "\nВыбрано факторов: k = " << k << "\n";

    // ===== ФАКТОРНЫЕ НАГРУЗКИ =====
    auto L = compute_factor_loadings(R, k);

    // Сохраняем в CSV
    std::vector<std::string> factor_names;
    for (int i = 0; i < k; ++i)
        factor_names.push_back("F" + std::to_string(i + 1));

    write_matrix_csv("results/factor_loadings.csv", factor_names, L);

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
        if (!printed) std::cout << "(нет сильных нагрузок)";
        std::cout << "\n";
    }

    std::cout << "\nФайл results/factor_loadings.csv сохранён.\n";

    return 0;
}
