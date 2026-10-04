#ifndef PASSWORD_GENERATOR_H
#define PASSWORD_GENERATOR_H

#include <string>

class PasswordGenerator {
public:
    static std::string generate(
        int length,
        bool uppercase = true,
        bool lowercase = true,
        bool numbers = true,
        bool symbols = true
    );
};

#endif