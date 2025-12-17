#pragma once
#include <string>
#include <vector>

// Читаем CSV (разделитель ';'), выбираем только нужные колонки,
// и возвращаем данные в виде матрицы double: rows x features_count.
bool read_students_csv(
    const std::string& path,
    const std::vector<std::string>& needed_columns,
    std::vector<std::vector<double>>& out_data,
    std::string& out_error
);
