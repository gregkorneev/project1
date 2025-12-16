#pragma once
#include <string>
#include <vector>

bool write_eigenvalues_csv(
    const std::string& path,
    const std::vector<double>& eigenvalues,
    const std::vector<double>& explained,
    const std::vector<double>& cumulative
);
