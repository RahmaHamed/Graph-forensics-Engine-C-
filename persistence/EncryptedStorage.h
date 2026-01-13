#pragma once
#include <openssl/evp.h>
#include <string>
#include <vector>
#include <iostream>

namespace graph {
class EncryptedStorage {
public:
    static inline const std::string SECRET_KEY = "01234567890123456789012345678901";
    static inline const std::string IV = "0123456789012345";

    static std::string encrypt(const std::string& plaintext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        int len;
        int ciphertext_len;
        std::vector<unsigned char> ciphertext(plaintext.size() + 128);

        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                           (unsigned char*)SECRET_KEY.c_str(), 
                           (unsigned char*)IV.c_str());

        EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                          (unsigned char*)plaintext.c_str(), plaintext.size());
        ciphertext_len = len;

        EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return std::string((char*)ciphertext.data(), ciphertext_len);
    }

    // NEW: Decryption Logic
    static std::string decrypt(const std::string& ciphertext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        int len;
        int plaintext_len;
        std::vector<unsigned char> plaintext(ciphertext.size());

        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                           (unsigned char*)SECRET_KEY.c_str(), 
                           (unsigned char*)IV.c_str());

        EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                          (unsigned char*)ciphertext.data(), ciphertext.size());
        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return std::string((char*)plaintext.data(), plaintext_len);
    }
};
}
