#include "crypto3des.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

static bool same_bytes(const std::vector<unsigned char>& a,
                       const std::vector<unsigned char>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

int main() {
    // Фиксированные пути (учебный вариант)
    const std::string input_plain_path = "data/students.csv";
    const std::string encrypted_path  = "results/students.enc";
    const std::string decrypted_path  = "results/students_dec.csv";

    // Фиксированный ключ (учебный пример)
    const std::string FIXED_KEY = "edu_3des_key";

    while (true) {
        std::cout << "\n=== Triple DES (3DES) ===\n";
        std::cout << "1 - Зашифровать файл\n";
        std::cout << "2 - Расшифровать файл\n";
        std::cout << "0 - Выход\n";
        std::cout << "Выберите действие: ";

        int choice = -1;
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Выход из программы.\n";
            break;
        }

        if (choice != 1 && choice != 2) {
            std::cout << "Некорректный выбор. Повторите ввод.\n";
            continue;
        }

        std::vector<unsigned char> input_data;
        std::string error;

        const std::string input_path =
            (choice == 1) ? input_plain_path : encrypted_path;

        if (!crypto3des::read_file(input_path, input_data, error)) {
            std::cout << "Ошибка чтения файла: " << error << "\n";
            continue;
        }

        std::cout << "Файл прочитан: " << input_path << "\n";
        std::cout << "Размер входных данных: " << input_data.size() << " байт\n";

        auto t0 = std::chrono::high_resolution_clock::now();

        if (choice == 1) {
            // ===== ШИФРОВАНИЕ =====
            std::vector<unsigned char> cipher;

            if (!crypto3des::encrypt_3des_cbc(input_data, FIXED_KEY, cipher, error)) {
                std::cout << "Ошибка шифрования: " << error << "\n";
                continue;
            }

            auto t1 = std::chrono::high_resolution_clock::now();

            if (!crypto3des::write_file(encrypted_path, cipher, error)) {
                std::cout << "Ошибка записи файла: " << error << "\n";
                continue;
            }

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            std::cout << "Шифрование выполнено успешно.\n";
            std::cout << "Результат сохранён в: " << encrypted_path << "\n";
            std::cout << "Размер шифртекста: " << cipher.size() << " байт\n";
            std::cout << "Время шифрования: " << ms << " мс\n";

            // Самопроверка
            std::vector<unsigned char> back;
            std::string error2;

            if (crypto3des::decrypt_3des_cbc(cipher, FIXED_KEY, back, error2)
                && same_bytes(input_data, back)) {
                std::cout << "Самопроверка: OK (decrypt(encrypt(data)) == data)\n";
            } else {
                std::cout << "Самопроверка: ошибка\n";
            }

        } else {
            // ===== ДЕШИФРОВАНИЕ =====
            std::vector<unsigned char> plain;

            if (!crypto3des::decrypt_3des_cbc(input_data, FIXED_KEY, plain, error)) {
                std::cout << "Ошибка дешифрования: " << error << "\n";
                continue;
            }

            auto t1 = std::chrono::high_resolution_clock::now();

            if (!crypto3des::write_file(decrypted_path, plain, error)) {
                std::cout << "Ошибка записи файла: " << error << "\n";
                continue;
            }

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            std::cout << "Дешифрование выполнено успешно.\n";
            std::cout << "Результат сохранён в: " << decrypted_path << "\n";
            std::cout << "Размер расшифрованных данных: " << plain.size() << " байт\n";
            std::cout << "Время дешифрования: " << ms << " мс\n";
        }
    }

    return 0;
}
