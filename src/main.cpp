#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "csv_reader.h"
#include "standardize.h"
#include "correlation.h"

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
        std::cerr << "Ошибка стандартизации: возможно, stddev=0 в каком-то столбце.\n";
        return 1;
    }
    std::cout << "Стандартизация выполнена.\n";

    // Считаем корреляционную матрицу
    auto R = correlation_matrix(X);

    std::cout << "\nКорреляционная матрица (первые 5x5):\n";
    std::cout << std::fixed << std::setprecision(3);

    int show = 5;
    for (int i = 0; i < show; ++i) {
        for (int j = 0; j < show; ++j) {
            std::cout << std::setw(8) << R[i][j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
