#ifndef PASSWORD_STRENGTH_H
#define PASSWORD_STRENGTH_H

#include <string>

struct PasswordStrengthResult {
    int score;
    std::string level;
};

class PasswordStrength {
public:
    static PasswordStrengthResult analyze(const std::string& password);
};

#endif