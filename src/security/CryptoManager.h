#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <string>
#include <vector>

struct EncryptedData {
    std::vector<unsigned char> nonce;
    std::vector<unsigned char> ciphertext;
    std::vector<unsigned char> tag;
};

class CryptoManager {
public:
    static EncryptedData encrypt(
        const std::string& plaintext,
        const std::vector<unsigned char>& key
    );

    static std::string decrypt(
        const EncryptedData& data,
        const std::vector<unsigned char>& key
    );

    static std::vector<unsigned char> generateNonce(
        size_t size = 12
    );
};

#endif