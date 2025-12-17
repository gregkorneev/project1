#pragma once

#include <string>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Пишет квадратную матрицу (например корреляционную) в CSV:
// Header: Feature,<label1>,<label2>,...
// Rows:   <label_i>,v1,v2,...
bool write_square_matrix_csv(
    const std::string& filepath,
    const std::vector<std::string>& labels,
    const Matrix& m,
    char separator = ','
);

// Пишет таблицу (строки x столбцы) в CSV:
// Header: Feature,<col1>,<col2>,...
// Rows:   <row_i>,v1,v2,...
bool write_table_csv(
    const std::string& filepath,
    const std::vector<std::string>& row_names,
    const std::vector<std::string>& col_names,
    const Matrix& data,
    char separator = ','
);
