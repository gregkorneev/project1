#pragma once
#include <vector>

// Стандартизация: (x - mean) / stddev по каждому столбцу.
// Возвращает false, если где-то stddev == 0 (деление на ноль).
bool standardize_inplace(
    std::vector<std::vector<double>>& data,
    std::vector<double>& out_mean,
    std::vector<double>& out_stddev
);
