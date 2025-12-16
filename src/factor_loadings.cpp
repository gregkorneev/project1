#include "factor_loadings.h"

#include <Eigen/Dense>
#include <cmath>

Matrix compute_factor_loadings(
    const std::vector<std::vector<double>>& R,
    int k
) {
    int m = (int)R.size();
    Matrix L(m, std::vector<double>(k, 0.0));

    if (m == 0 || k <= 0) return L;

    Eigen::MatrixXd M(m, m);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < m; ++j)
            M(i, j) = R[i][j];

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(M);
    Eigen::VectorXd evals = solver.eigenvalues();   // по возрастанию
    Eigen::MatrixXd evecs = solver.eigenvectors(); // столбцы — собственные векторы

    // Берём k наибольших собственных значений/векторов
    for (int f = 0; f < k; ++f) {
        int idx = m - 1 - f; // с конца (самые большие)
        double lambda = evals(idx);

        double scale = (lambda > 0.0) ? std::sqrt(lambda) : 0.0;

        for (int i = 0; i < m; ++i) {
            L[i][f] = evecs(i, idx) * scale;
        }
    }

    return L;
}
