#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "csv_reader.h"
#include "standardize.h"
#include "correlation.h"
#include "csv_writer.h"
#include "eigen_analysis.h"
#include "eigen_results_writer.h"
#include "factor_loadings.h"

using std::string;
using std::vector;

static vector<string> make_factor_names(int k)
{
    vector<string> names;
    for (int i = 1; i <= k; ++i)
        names.push_back("F" + std::to_string(i));
    return names;
}

static vector<string> make_factor_titles(int k)
{
    // Названия-объяснения (латиница, чтобы не было проблем с кодировкой)
    vector<string> base = {
        "F1(social_activity)",
        "F2(achievement)",
        "F3(family_leisure)",
        "F4(age_absences)",
        "F5(study)",
        "F6(other)"
    };

    vector<string> out;
    for (int i = 0; i < k; ++i)
    {
        if (i < (int)base.size()) out.push_back(base[i]);
        else out.push_back("F" + std::to_string(i + 1));
    }
    return out;
}

static void print_loadings_table_manual(
    const vector<string>& features,
    const vector<string>& factor_titles,
    const Matrix& loadings)
{
    const int name_w = 12;
    const int col_w = 18;

    std::cout << "\nFactor loadings matrix (features x factors):\n";

    // Header
    std::cout << std::left << std::setw(name_w) << "Feature";
    for (size_t j = 0; j < factor_titles.size(); ++j)
        std::cout << std::left << std::setw(col_w) << factor_titles[j];
    std::cout << "\n";

    // Rows
    std::cout << std::fixed << std::setprecision(3);
    for (size_t i = 0; i < features.size(); ++i)
    {
        std::cout << std::left << std::setw(name_w) << features[i];
        for (size_t j = 0; j < factor_titles.size(); ++j)
            std::cout << std::left << std::setw(col_w) << loadings[i][j];
        std::cout << "\n";
    }
}

static void print_dominant_factor_per_feature(
    const vector<string>& features,
    const Matrix& loadings,
    const vector<string>& factor_titles)
{
    std::cout << "\nDominant factor per feature (max |loading|):\n";
    std::cout << std::fixed << std::setprecision(3);

    for (size_t i = 0; i < features.size(); ++i)
    {
        int best_j = 0;
        double best_abs = std::fabs(loadings[i][0]);

        for (size_t j = 1; j < loadings[i].size(); ++j)
        {
            double a = std::fabs(loadings[i][j]);
            if (a > best_abs)
            {
                best_abs = a;
                best_j = (int)j;
            }
        }

        std::cout << "- " << features[i]
                  << " -> " << factor_titles[best_j]
                  << " = " << loadings[i][best_j] << "\n";
    }
}

int main()
{
    const string input_csv = "data/students.csv";

    const vector<string> features = {
        "age", "studytime", "failures", "famrel", "freetime",
        "goout", "Dalc", "Walc", "absences", "G3"
    };

    std::filesystem::create_directories("results");
    vector<string> saved_files;

    // ===== Read CSV =====
    Matrix X;
    string error;
    if (!read_students_csv(input_csv, features, X, error))
    {
        std::cerr << "CSV read error: " << error << "\n";
        return 1;
    }

    std::cout << "Loaded rows: " << X.size() << "\n";
    std::cout << "Number of features: " << features.size() << "\n";

    // ===== Standardize =====
    vector<double> mean, stddev;
    if (!standardize_inplace(X, mean, stddev))
    {
        std::cerr << "Standardization error\n";
        return 1;
    }
    std::cout << "Standardization done.\n";

    // =================================================
    // Variant A: Correlation matrix
    // =================================================
    Matrix corr = correlation_matrix(X);

    const string corr_path = "results/correlation_matrix.csv";
    if (!write_square_matrix_csv(corr_path, features, corr))
    {
        std::cerr << "Failed to save correlation matrix.\n";
        return 1;
    }
    saved_files.push_back(corr_path);
    std::cout << "Correlation matrix saved to: " << corr_path << "\n";

    // =================================================
    // Variant B: Eigen analysis + Factor loadings
    // =================================================
    EigenReport report = analyze_eigenvalues(corr);

    const string eig_path = "results/eigenvalues.csv";
    if (!write_eigenvalues_csv(eig_path, report.eigenvalues, report.explained, report.cumulative))
    {
        std::cerr << "Failed to save eigenvalues table.\n";
        return 1;
    }
    saved_files.push_back(eig_path);
    std::cout << "Eigenvalues report saved to: " << eig_path << "\n";

    std::cout << "\nEigenvalues / explained variance:\n";
    std::cout << "Factor  Eigenvalue  Explained(%)  Cumulative(%)\n";
    std::cout << std::fixed << std::setprecision(4);
    for (size_t i = 0; i < report.eigenvalues.size(); ++i)
    {
        std::cout << std::setw(6) << (i + 1) << "  "
                  << std::setw(9) << report.eigenvalues[i] << "  "
                  << std::setw(11) << report.explained[i] << "  "
                  << std::setw(12) << report.cumulative[i] << "\n";
    }

    const int k = report.k_for_75;
    std::cout << "\nChosen number of factors (>=75%): k = " << k << "\n";

    Matrix loadings = compute_factor_loadings(corr, k);

    const string load_path = "results/factor_loadings.csv";
    if (!write_table_csv(load_path, features, make_factor_names(k), loadings))
    {
        std::cerr << "Failed to save factor loadings.\n";
        return 1;
    }
    saved_files.push_back(load_path);
    std::cout << "Factor loadings saved to: " << load_path << "\n";

    const vector<string> factor_titles = make_factor_titles(k);

    // Печатаем таблицу вручную (без зависимости от чужих функций)
    print_loadings_table_manual(features, factor_titles, loadings);

    std::cout << "\nHint: |loading| >= 0.4 is considered significant.\n";

    print_dominant_factor_per_feature(features, loadings, factor_titles);

    // ===== Summary =====
    std::cout << "\nSaved files in results:\n";
    for (const auto& f : saved_files)
        std::cout << "- " << f << "\n";

    return 0;
}
