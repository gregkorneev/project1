#include "eigen_results_writer.h"
#include <fstream>

bool write_eigenvalues_csv(
    const std::string& path,
    const std::vector<double>& eigenvalues,
    const std::vector<double>& explained,
    const std::vector<double>& cumulative
) {
    if (eigenvalues.size() != explained.size() || eigenvalues.size() != cumulative.size())
        return false;

    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << "factor,eigenvalue,explained,cumulative\n";
    for (size_t i = 0; i < eigenvalues.size(); ++i) {
        file << (i + 1) << ","
             << eigenvalues[i] << ","
             << explained[i] << ","
             << cumulative[i] << "\n";
    }
    return true;
}
