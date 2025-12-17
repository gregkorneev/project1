#include "eigen_analysis.h"

#include <Eigen/Dense>
#include <algorithm>
#include <numeric>

EigenReport analyze_eigenvalues(const std::vector<std::vector<double>>& R)
{
    EigenReport rep;

    int m = (int)R.size();
    if (m == 0) return rep;

    Eigen::MatrixXd M(m, m);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            M(i, j) = R[i][j];
        }
    }

    // Для симметричных матриц (а корреляционная симметрична) используем SelfAdjointEigenSolver
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(M);
    Eigen::VectorXd evals = solver.eigenvalues(); // по возрастанию

    // Перенесём в vector и развернём, чтобы было по убыванию
    rep.eigenvalues.resize(m);
    for (int i = 0; i < m; ++i) {
        rep.eigenvalues[i] = evals(i);
    }
    std::reverse(rep.eigenvalues.begin(), rep.eigenvalues.end());

    double sum = 0.0;
    for (double x : rep.eigenvalues) sum += x;
    if (sum == 0.0) return rep;

    rep.explained.resize(m);
    rep.cumulative.resize(m);

    double cum = 0.0;
    for (int i = 0; i < m; ++i) {
        rep.explained[i] = rep.eigenvalues[i] / sum;
        cum += rep.explained[i];
        rep.cumulative[i] = cum;
    }

    // минимальное k, чтобы cumulative >= 0.75
    rep.k_for_75 = 0;
    for (int i = 0; i < m; ++i) {
        if (rep.cumulative[i] >= 0.75) {
            rep.k_for_75 = i + 1; // потому что i с нуля
            break;
        }
    }

    return rep;
}
