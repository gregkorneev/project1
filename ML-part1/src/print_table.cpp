#include "print_table.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

void print_factor_loadings_table(
    const std::vector<std::string>& feature_names,
    const Matrix& loadings,
    const std::vector<std::string>& factor_titles
)
{
    if (feature_names.size() != loadings.size())
    {
        std::cout << "Ошибка: размеры feature_names и loadings не совпадают.\n";
        return;
    }
    if (!loadings.empty() && factor_titles.size() != loadings[0].size())
    {
        std::cout << "Ошибка: число факторов в factor_titles не совпадает с loadings.\n";
        return;
    }

    // Ширины колонок для аккуратного вывода
    const int name_w = 12;
    const int col_w  = 14; // побольше, чтобы влезло "F1(....)"

    std::cout << "\nМатрица факторных нагрузок (признаки x факторы):\n";

    // Заголовок
    std::cout << std::left << std::setw(name_w) << "Признак";
    for (const auto& t : factor_titles)
    {
        std::cout << std::right << std::setw(col_w) << t;
    }
    std::cout << "\n";

    // Строки
    std::cout << std::fixed << std::setprecision(3);
    for (size_t i = 0; i < loadings.size(); ++i)
    {
        std::cout << std::left << std::setw(name_w) << feature_names[i];
        for (size_t j = 0; j < loadings[i].size(); ++j)
        {
            std::cout << std::right << std::setw(col_w) << loadings[i][j];
        }
        std::cout << "\n";
    }
}
