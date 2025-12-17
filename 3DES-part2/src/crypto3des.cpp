#include "crypto3des.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>

#include <openssl/evp.h>
#include <openssl/sha.h>

namespace crypto3des {

static void derive_key_iv_from_passphrase(
    const std::string& passphrase,
    unsigned char key24[24],
    unsigned char iv8[8]
) {
    // Очень простая (учебная) деривация:
    // hash = SHA256(passphrase)
    // key = первые 24 байта hash
    // iv  = последние 8 байт hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(passphrase.data()),
           passphrase.size(),
           hash);

    for (int i = 0; i < 24; ++i) key24[i] = hash[i];
    for (int i = 0; i < 8;  ++i) iv8[i]  = hash[SHA256_DIGEST_LENGTH - 8 + i];
}

bool encrypt_3des_cbc(
    const std::vector<unsigned char>& plain,
    const std::string& passphrase,
    std::vector<unsigned char>& cipher,
    std::string& error
) {
    cipher.clear();
    error.clear();

    unsigned char key[24];
    unsigned char iv[8];
    derive_key_iv_from_passphrase(passphrase, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        error = "Не удалось создать EVP_CIPHER_CTX";
        return false;
    }

    const EVP_CIPHER* alg = EVP_des_ede3_cbc(); // 3DES CBC

    if (EVP_EncryptInit_ex(ctx, alg, nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_EncryptInit_ex: ошибка инициализации";
        return false;
    }

    // Выделяем место с запасом: plain + block_size
    cipher.resize(plain.size() + EVP_CIPHER_block_size(alg));

    int out_len1 = 0;
    if (EVP_EncryptUpdate(ctx,
                          cipher.data(), &out_len1,
                          plain.data(), static_cast<int>(plain.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_EncryptUpdate: ошибка шифрования";
        return false;
    }

    int out_len2 = 0;
    if (EVP_EncryptFinal_ex(ctx, cipher.data() + out_len1, &out_len2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_EncryptFinal_ex: ошибка финализации (padding)";
        return false;
    }

    cipher.resize(static_cast<size_t>(out_len1 + out_len2));
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_3des_cbc(
    const std::vector<unsigned char>& cipher,
    const std::string& passphrase,
    std::vector<unsigned char>& plain,
    std::string& error
) {
    plain.clear();
    error.clear();

    unsigned char key[24];
    unsigned char iv[8];
    derive_key_iv_from_passphrase(passphrase, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        error = "Не удалось создать EVP_CIPHER_CTX";
        return false;
    }

    const EVP_CIPHER* alg = EVP_des_ede3_cbc();

    if (EVP_DecryptInit_ex(ctx, alg, nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_DecryptInit_ex: ошибка инициализации";
        return false;
    }

    // plain не будет больше cipher
    plain.resize(cipher.size());

    int out_len1 = 0;
    if (EVP_DecryptUpdate(ctx,
                          plain.data(), &out_len1,
                          cipher.data(), static_cast<int>(cipher.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_DecryptUpdate: ошибка дешифрования";
        return false;
    }

    int out_len2 = 0;
    if (EVP_DecryptFinal_ex(ctx, plain.data() + out_len1, &out_len2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        error = "EVP_DecryptFinal_ex: ошибка финализации (возможно, неверный пароль/повреждённые данные)";
        return false;
    }

    plain.resize(static_cast<size_t>(out_len1 + out_len2));
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool read_file(const std::string& path, std::vector<unsigned char>& data, std::string& error) {
    data.clear();
    error.clear();

    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "Не удалось открыть файл для чтения: " + path;
        return false;
    }

    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    in.seekg(0, std::ios::beg);

    if (size < 0) {
        error = "Не удалось определить размер файла: " + path;
        return false;
    }

    data.resize(static_cast<size_t>(size));
    if (!data.empty()) {
        in.read(reinterpret_cast<char*>(data.data()), size);
        if (!in) {
            error = "Ошибка чтения файла: " + path;
            return false;
        }
    }

    return true;
}

bool write_file(const std::string& path, const std::vector<unsigned char>& data, std::string& error) {
    error.clear();

    std::ofstream out(path, std::ios::binary);
    if (!out) {
        error = "Не удалось открыть файл для записи: " + path;
        return false;
    }

    if (!data.empty()) {
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        if (!out) {
            error = "Ошибка записи файла: " + path;
            return false;
        }
    }

    return true;
}

std::string to_hex_preview(const std::vector<unsigned char>& data, size_t max_bytes) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    size_t n = data.size();
    size_t take = (n < max_bytes) ? n : max_bytes;

    for (size_t i = 0; i < take; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
        if (i + 1 != take) ss << " ";
    }

    if (n > take) ss << " ...";
    return ss.str();
}

} // namespace crypto3des
