#include "CryptoManager.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

std::vector<unsigned char> CryptoManager::generateNonce(
    size_t size) {

    std::vector<unsigned char> nonce(size);

    if (RAND_bytes(
            nonce.data(),
            static_cast<int>(nonce.size())) != 1) {

        throw std::runtime_error(
            "Failed to generate secure nonce."
        );
    }

    return nonce;
}

EncryptedData CryptoManager::encrypt(
    const std::string& plaintext,
    const std::vector<unsigned char>& key) {

    if (key.size() != 32)
        throw std::invalid_argument(
            "AES-256 requires a 32-byte key."
        );

    EncryptedData result;

    result.nonce = generateNonce(12);
    result.ciphertext.resize(plaintext.size());
    result.tag.resize(16);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
        throw std::runtime_error(
            "Failed to create encryption context."
        );

    int length = 0;
    int ciphertextLength = 0;

    if (EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            nullptr,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to initialize AES-256-GCM."
        );
    }

    if (EVP_EncryptInit_ex(
            ctx,
            nullptr,
            nullptr,
            key.data(),
            result.nonce.data()) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to initialize encryption key."
        );
    }

    if (EVP_EncryptUpdate(
            ctx,
            result.ciphertext.data(),
            &length,
            reinterpret_cast<const unsigned char*>(
                plaintext.data()),
            static_cast<int>(plaintext.size())) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Encryption failed."
        );
    }

    ciphertextLength = length;

    if (EVP_EncryptFinal_ex(
            ctx,
            result.ciphertext.data() + ciphertextLength,
            &length) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Encryption finalization failed."
        );
    }

    ciphertextLength += length;
    result.ciphertext.resize(ciphertextLength);

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_GET_TAG,
            16,
            result.tag.data()) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to generate authentication tag."
        );
    }

    EVP_CIPHER_CTX_free(ctx);

    return result;
}

std::string CryptoManager::decrypt(
    const EncryptedData& data,
    const std::vector<unsigned char>& key) {

    if (key.size() != 32)
        throw std::invalid_argument(
            "AES-256 requires a 32-byte key."
        );

    if (data.nonce.size() != 12)
        throw std::invalid_argument(
            "Invalid nonce size."
        );

    if (data.tag.size() != 16)
        throw std::invalid_argument(
            "Invalid authentication tag size."
        );

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
        throw std::runtime_error(
            "Failed to create decryption context."
        );

    std::vector<unsigned char> plaintext(
        data.ciphertext.size()
    );

    int length = 0;
    int plaintextLength = 0;

    if (EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            nullptr,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to initialize AES-256-GCM."
        );
    }

    if (EVP_DecryptInit_ex(
            ctx,
            nullptr,
            nullptr,
            key.data(),
            data.nonce.data()) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to initialize decryption key."
        );
    }

    if (EVP_DecryptUpdate(
            ctx,
            plaintext.data(),
            &length,
            data.ciphertext.data(),
            static_cast<int>(data.ciphertext.size())) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Decryption failed."
        );
    }

    plaintextLength = length;

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_TAG,
            16,
            const_cast<unsigned char*>(
                data.tag.data())) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Failed to set authentication tag."
        );
    }

    if (EVP_DecryptFinal_ex(
            ctx,
            plaintext.data() + plaintextLength,
            &length) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error(
            "Authentication failed. Wrong key or modified data."
        );
    }

    plaintextLength += length;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(
        reinterpret_cast<char*>(plaintext.data()),
        plaintextLength
    );
}