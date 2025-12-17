#pragma once

#include <string>
#include <vector>

// Простые функции для 3DES (Triple DES) через OpenSSL EVP.
// Шифруем/дешифруем байты. Ключ и IV получаем из "пароля" (passphrase).
namespace crypto3des {

    // Шифрование данных (3DES-CBC + PKCS#7 padding).
    // passphrase -> ключ(24 байта) + iv(8 байт) через SHA-256.
    bool encrypt_3des_cbc(
        const std::vector<unsigned char>& plain,
        const std::string& passphrase,
        std::vector<unsigned char>& cipher,
        std::string& error
    );

    // Дешифрование данных (3DES-CBC + PKCS#7 padding).
    bool decrypt_3des_cbc(
        const std::vector<unsigned char>& cipher,
        const std::string& passphrase,
        std::vector<unsigned char>& plain,
        std::string& error
    );

    // Утилиты для чтения/записи файла целиком.
    bool read_file(const std::string& path, std::vector<unsigned char>& data, std::string& error);
    bool write_file(const std::string& path, const std::vector<unsigned char>& data, std::string& error);

    // Для красивого вывода примера (hex первых N байт).
    std::string to_hex_preview(const std::vector<unsigned char>& data, size_t max_bytes = 32);
}
