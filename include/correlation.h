#pragma once
#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Считает корреляционную матрицу m×m для стандартизированных данных (n×m).
Matrix correlation_matrix(const Matrix& z);
