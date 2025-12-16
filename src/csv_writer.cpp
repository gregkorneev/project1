#include "csv_writer.h"

#include <fstream>
#include <iomanip>
#include <iostream>

// Экранирование для CSV: если есть разделитель/кавычки/перевод строки — берём в кавычки,
// а кавычки внутри удваиваем.
static std::string csv_escape(const std::string& s, char separator)
{
    bool need_quotes = false;
    for (char c : s)
    {
        if (c == separator || c == '"' || c == '\n' || c == '\r')
        {
            need_quotes = true;
            break;
        }
    }

    if (!need_quotes) return s;

    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s)
    {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out.push_back('"');
    return out;
}

bool write_table_csv(
    const std::string& filepath,
    const std::vector<std::string>& row_names,
    const std::vector<std::string>& col_names,
    const Matrix& data,
    char separator
)
{
    if (data.size() != row_names.size())
    {
        std::cerr << "Ошибка: write_table_csv: число строк data не совпадает с row_names.\n";
        return false;
    }
    if (!data.empty() && data[0].size() != col_names.size())
    {
        std::cerr << "Ошибка: write_table_csv: число столбцов data не совпадает с col_names.\n";
        return false;
    }

    std::ofstream out(filepath);
    if (!out.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл для записи: " << filepath << "\n";
        return false;
    }

    // Заголовок
    out << "Feature";
    for (const auto& col : col_names)
    {
        out << separator << csv_escape(col, separator);
    }
    out << "\n";

    // Данные
    out << std::fixed << std::setprecision(3);
    for (size_t i = 0; i < data.size(); ++i)
    {
        out << csv_escape(row_names[i], separator);
        for (size_t j = 0; j < col_names.size(); ++j)
        {
            out << separator << data[i][j];
        }
        out << "\n";
    }

    return true;
}

bool write_square_matrix_csv(
    const std::string& filepath,
    const std::vector<std::string>& labels,
    const Matrix& m,
    char separator
)
{
    // Проверка размеров
    if (m.size() != labels.size())
    {
        std::cerr << "Ошибка: write_square_matrix_csv: матрица не квадратная или labels не совпадают.\n";
        return false;
    }
    for (const auto& row : m)
    {
        if (row.size() != labels.size())
        {
            std::cerr << "Ошибка: write_square_matrix_csv: матрица не квадратная.\n";
            return false;
        }
    }

    // Пишем как таблицу: Feature + labels
    return write_table_csv(filepath, labels, labels, m, separator);
}
