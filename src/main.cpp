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
        std::cerr << "Ошибка стандартизации: возможно, в каком-то столбце нулевая дисперсия.\n";
        return 1;
    }
    std::cout << "Стандартизация выполнена.\n";

    // Корреляционная матрица
    auto R = correlation_matrix(X);

    // Папка results/
    std::filesystem::create_directory("results");

    // Сохраняем матрицу корреляций
    if (!write_matrix_csv("results/correlation_matrix.csv", features, R)) {
        std::cerr << "Не удалось сохранить файл: results/correlation_matrix.csv\n";
        return 1;
    }
    std::cout << "Корреляционная матрица сохранена в: results/correlation_matrix.csv\n";

    // Анализ собственных значений
    EigenReport rep = analyze_eigenvalues(R);

    // Печать таблицы
    std::cout << "\nСобственные значения и объяснённая дисперсия:\n";
    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Фактор  Собств.знач.  Доля(%)   Накопл.(%)\n";
    for (size_t i = 0; i < rep.eigenvalues.size(); ++i) {
        double pct = rep.explained[i] * 100.0;
        double cpct = rep.cumulative[i] * 100.0;

        std::cout << std::setw(6) << (i + 1) << "  "
                  << std::setw(11) << rep.eigenvalues[i] << "  "
                  << std::setw(7) << pct << "  "
                  << std::setw(9) << cpct << "\n";
    }

    std::cout << "\nМинимальное число факторов для объяснения >= 75% дисперсии: k = "
              << rep.k_for_75 << "\n";

    if (rep.k_for_75 > 0) {
        std::cout << "Накопленная доля для k: "
                  << (rep.cumulative[rep.k_for_75 - 1] * 100.0) << "%\n";
    }

    // Сохраняем eigenvalues в CSV (заголовки оставляем на английском для удобства)
    if (!write_eigenvalues_csv("results/eigenvalues.csv", rep.eigenvalues, rep.explained, rep.cumulative)) {
        std::cerr << "Не удалось сохранить файл: results/eigenvalues.csv\n";
        return 1;
    }
    std::cout << "Файл results/eigenvalues.csv сохранён.\n";

    return 0;
}
