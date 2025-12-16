#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>

#include "csv_reader.h"
#include "standardize.h"
#include "correlation.h"
#include "csv_writer.h"

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

    // Корреляционная матрица
    auto R = correlation_matrix(X);

    // Создаём папку results, если её нет
    std::filesystem::create_directory("results");

    // Сохраняем в CSV
    if (!write_matrix_csv("results/correlation_matrix.csv", features, R)) {
        std::cerr << "Не удалось сохранить correlation_matrix.csv\n";
        return 1;
    }

    std::cout << "Корреляционная матрица сохранена в results/correlation_matrix.csv\n";

    // Вывод в консоль (первые 5x5) с заголовками
    std::cout << "\nКорреляционная матрица (первые 5x5):\n";
    std::cout << std::fixed << std::setprecision(3);

    std::cout << std::setw(12) << "";
    for (int j = 0; j < 5; ++j)
        std::cout << std::setw(10) << features[j];
    std::cout << "\n";

    for (int i = 0; i < 5; ++i) {
        std::cout << std::setw(12) << features[i];
        for (int j = 0; j < 5; ++j) {
            std::cout << std::setw(10) << R[i][j];
        }
        std::cout << "\n";
    }

    return 0;
}
