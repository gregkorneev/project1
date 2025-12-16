#include "csv_reader.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

// Убираем пробелы по краям
static std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() &&
          (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        start++;

    size_t end = s.size();
    while (end > start &&
          (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r'))
        end--;

    return s.substr(start, end - start);
}

// Убираем UTF-8 BOM, если есть
static std::string remove_bom(const std::string& s)
{
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
        return s.substr(3);
    return s;
}

// Определяем разделитель по заголовку
static char detect_delimiter(const std::string& header_line)
{
    int commas = 0, semicolons = 0;
    for (char c : header_line) {
        if (c == ',') commas++;
        if (c == ';') semicolons++;
    }
    return (semicolons > commas) ? ';' : ',';
}

// Разбиваем строку CSV с учётом кавычек
static std::vector<std::string> split_csv_line(const std::string& line, char delim)
{
    std::vector<std::string> parts;
    std::string cur;
    bool in_quotes = false;

    for (char c : line) {
        if (c == '"') {
            in_quotes = !in_quotes;
            continue;
        }
        if (c == delim && !in_quotes) {
            parts.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    parts.push_back(cur);
    return parts;
}

static bool to_double(const std::string& s, double& out)
{
    try {
        size_t pos = 0;
        out = std::stod(s, &pos);
        return pos == s.size();
    } catch (...) {
        return false;
    }
}

bool read_students_csv(
    const std::string& path,
    const std::vector<std::string>& needed_columns,
    std::vector<std::vector<double>>& out_data,
    std::string& out_error
) {
    out_data.clear();
    out_error.clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        out_error = "Не удалось открыть файл: " + path;
        return false;
    }

    std::string header_line;
    if (!std::getline(file, header_line)) {
        out_error = "Файл пустой.";
        return false;
    }

    char delim = detect_delimiter(header_line);
    std::vector<std::string> header = split_csv_line(header_line, delim);

    // Чистим и печатаем заголовки
    std::cout << "Найденные колонки (" << header.size() << "):\n";
    for (size_t i = 0; i < header.size(); ++i) {
        header[i] = trim(remove_bom(header[i]));
        std::cout << "  [" << i << "] '" << header[i] << "'\n";
    }

    std::unordered_map<std::string, int> index;
    for (int i = 0; i < (int)header.size(); ++i) {
        index[header[i]] = i;
    }

    std::vector<int> needed_idx;
    for (const std::string& name_raw : needed_columns) {
        std::string name = trim(name_raw);
        if (index.find(name) == index.end()) {
            out_error = "В CSV нет колонки: " + name;
            return false;
        }
        needed_idx.push_back(index[name]);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> cells = split_csv_line(line, delim);
        if (cells.size() < header.size()) continue;

        std::vector<double> row;
        bool ok = true;

        for (int col : needed_idx) {
            std::string cell = trim(cells[col]);
            double val = 0.0;
            if (cell.empty() || !to_double(cell, val)) {
                ok = false;
                break;
            }
            row.push_back(val);
        }

        if (ok)
            out_data.push_back(row);
    }

    if (out_data.empty()) {
        out_error = "Нет данных после чтения строк.";
        return false;
    }

    return true;
}
