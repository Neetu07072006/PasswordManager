#include "PasswordStrength.h"
#include <cctype>
#include <algorithm>

PasswordStrengthResult PasswordStrength::analyze(
    const std::string& password) {

    int score = 0;

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (unsigned char ch : password) {
        if (std::isupper(ch))
            hasUpper = true;
        else if (std::islower(ch))
            hasLower = true;
        else if (std::isdigit(ch))
            hasDigit = true;
        else
            hasSymbol = true;
    }

    if (password.length() >= 8)
        score += 2;

    if (password.length() >= 12)
        score += 2;

    if (password.length() >= 16)
        score += 1;

    if (hasUpper)
        score += 1;

    if (hasLower)
        score += 1;

    if (hasDigit)
        score += 1;

    if (hasSymbol)
        score += 1;

    for (size_t i = 1; i < password.length(); ++i) {
        if (password[i] == password[i - 1]) {
            score -= 1;
            break;
        }
    }

    std::string lowerPassword = password;

    std::transform(
        lowerPassword.begin(),
        lowerPassword.end(),
        lowerPassword.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        }
    );

    const std::string weakPasswords[] = {
        "password",
        "password123",
        "123456",
        "12345678",
        "qwerty",
        "admin",
        "welcome",
        "letmein"
    };

    for (const auto& weak : weakPasswords) {
        if (lowerPassword == weak) {
            score = 0;
            break;
        }
    }

    score = std::max(0, std::min(score, 10));

    std::string level;

    if (score <= 2)
        level = "Very Weak";
    else if (score <= 4)
        level = "Weak";
    else if (score <= 6)
        level = "Moderate";
    else if (score <= 8)
        level = "Strong";
    else
        level = "Very Strong";

    return {score, level};
}