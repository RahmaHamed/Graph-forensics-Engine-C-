#pragma once
#include <string>
#include <iomanip>
#include <sstream>
#include <openssl/evp.h>

namespace graph {
class Guard {
public:
    static std::string generateSHA256(const std::string& data) {
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        unsigned char hash[32];
        unsigned int len = 0;

        EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
        EVP_DigestUpdate(ctx, data.c_str(), data.size());
        EVP_DigestFinal_ex(ctx, hash, &len);
        EVP_MD_CTX_free(ctx);

        std::stringstream ss;
        for(unsigned int i = 0; i < len; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
};
}
