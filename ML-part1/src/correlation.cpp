#include "correlation.h"

Matrix correlation_matrix(const Matrix& z)
{
    int n = (int)z.size();
    int m = (int)z[0].size();

    Matrix R(m, std::vector<double>(m, 0.0));

    // R = (Z^T * Z) / (n - 1)
    for (int i = 0; i < m; ++i) {
        for (int j = i; j < m; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += z[k][i] * z[k][j];
            }
            double val = sum / (n - 1);
            R[i][j] = val;
            R[j][i] = val; // симметрия
        }
    }

    return R;
}
