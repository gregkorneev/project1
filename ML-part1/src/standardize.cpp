#include "standardize.h"
#include <cmath>

bool standardize_inplace(
    std::vector<std::vector<double>>& data,
    std::vector<double>& out_mean,
    std::vector<double>& out_stddev
) {
    if (data.empty()) return false;
    int n = (int)data.size();
    int m = (int)data[0].size();
    if (m == 0) return false;

    out_mean.assign(m, 0.0);
    out_stddev.assign(m, 0.0);

    // Среднее по столбцам
    for (int j = 0; j < m; ++j) {
        double sum = 0.0;
        for (int i = 0; i < n; ++i) {
            sum += data[i][j];
        }
        out_mean[j] = sum / n;
    }

    // Стандартное отклонение по столбцам (несмещённое, делим на n-1)
    for (int j = 0; j < m; ++j) {
        double s2 = 0.0;
        for (int i = 0; i < n; ++i) {
            double d = data[i][j] - out_mean[j];
            s2 += d * d;
        }
        if (n < 2) return false;
        out_stddev[j] = std::sqrt(s2 / (n - 1));
        if (out_stddev[j] == 0.0) {
            return false; // нельзя стандартизировать столбец без разброса
        }
    }

    // Преобразование (x - mean) / stddev
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            data[i][j] = (data[i][j] - out_mean[j]) / out_stddev[j];
        }
    }

    return true;
}
