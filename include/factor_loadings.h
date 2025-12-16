#pragma once
#include <vector>

// Матрица факторных нагрузок: features × k
using Matrix = std::vector<std::vector<double>>;

// R — корреляционная матрица (m×m)
// k — число факторов
// Возвращает матрицу нагрузок (m×k)
Matrix compute_factor_loadings(
    const std::vector<std::vector<double>>& R,
    int k
);
