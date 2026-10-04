#include "KeyDerivation.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <stdexcept>

std::vector<unsigned char> KeyDerivation::generateSalt(size_t size) {
    std::vector<unsigned char> salt(size);

    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1)
        throw std::runtime_error("Failed to generate secure salt.");

    return salt;
}

std::vector<unsigned char> KeyDerivation::deriveKey(
    const std::string& password,
    const std::vector<unsigned char>& salt,
    int iterations,
    size_t keyLength) {

    if (password.empty())
        throw std::invalid_argument("Password cannot be empty.");

    if (salt.empty())
        throw std::invalid_argument("Salt cannot be empty.");

    std::vector<unsigned char> key(keyLength);

    EVP_KDF* kdf = EVP_KDF_fetch(nullptr, "PBKDF2", nullptr);

    if (!kdf)
        throw std::runtime_error("Failed to initialize PBKDF2.");

    EVP_KDF_CTX* ctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);

    if (!ctx)
        throw std::runtime_error("Failed to create KDF context.");

    int64_t iterationCount = iterations;

    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_PASSWORD,
            const_cast<char*>(password.data()),
            password.size()
        ),
        OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_SALT,
            const_cast<unsigned char*>(salt.data()),
            salt.size()
        ),
        OSSL_PARAM_construct_int64(
            OSSL_KDF_PARAM_ITER,
            &iterationCount
        ),
        OSSL_PARAM_construct_utf8_string(
            OSSL_KDF_PARAM_DIGEST,
            const_cast<char*>("SHA256"),
            0
        ),
        OSSL_PARAM_construct_end()
    };

    if (EVP_KDF_derive(
            ctx,
            key.data(),
            key.size(),
            params) != 1) {

        EVP_KDF_CTX_free(ctx);
        throw std::runtime_error("PBKDF2 key derivation failed.");
    }

    EVP_KDF_CTX_free(ctx);

    return key;
}