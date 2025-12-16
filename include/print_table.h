#pragma once

#include <string>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Печать матрицы факторных нагрузок (признаки x факторы) с пояснениями к факторам.
void print_factor_loadings_table(
    const std::vector<std::string>& feature_names,
    const Matrix& loadings,
    const std::vector<std::string>& factor_titles
);
