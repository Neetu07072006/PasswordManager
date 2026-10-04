#ifndef KEY_DERIVATION_H
#define KEY_DERIVATION_H

#include <string>
#include <vector>

class KeyDerivation {
public:
    static std::vector<unsigned char> generateSalt(size_t size = 16);

    static std::vector<unsigned char> deriveKey(
        const std::string& password,
        const std::vector<unsigned char>& salt,
        int iterations = 600000,
        size_t keyLength = 32
    );
};

#endif