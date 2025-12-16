#include "csv_writer.h"
#include <fstream>

bool write_matrix_csv(
    const std::string& path,
    const std::vector<std::string>& headers,
    const Matrix& M
) {
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    int m = (int)headers.size();

    // Первая строка: пустая ячейка + названия признаков
    file << ",";
    for (int j = 0; j < m; ++j) {
        file << headers[j];
        if (j + 1 < m) file << ",";
    }
    file << "\n";

    // Строки матрицы
    for (int i = 0; i < m; ++i) {
        file << headers[i] << ",";
        for (int j = 0; j < m; ++j) {
            file << M[i][j];
            if (j + 1 < m) file << ",";
        }
        file << "\n";
    }

    return true;
}
