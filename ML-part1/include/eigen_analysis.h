#pragma once
#include <vector>

// Результаты анализа собственных значений
struct EigenReport {
    std::vector<double> eigenvalues;   // lambda_i (по убыванию)
    std::vector<double> explained;     // lambda_i / sum
    std::vector<double> cumulative;    // накопленная сумма explained
    int k_for_75 = 0;                  // минимальное k, чтобы cumulative[k-1] >= 0.75
};

// На вход: корреляционная матрица R (m x m)
// На выход: EigenReport (eigenvalues, explained variance, cumulative и k)
EigenReport analyze_eigenvalues(const std::vector<std::vector<double>>& R);
