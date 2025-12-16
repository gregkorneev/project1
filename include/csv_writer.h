#pragma once
#include <vector>
#include <string>

using Matrix = std::vector<std::vector<double>>;

// Сохраняет матрицу с заголовками в CSV
bool write_matrix_csv(
    const std::string& path,
    const std::vector<std::string>& headers,
    const Matrix& M
);
