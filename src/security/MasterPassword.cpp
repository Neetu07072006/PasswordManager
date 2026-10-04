#include "MasterPassword.h"

bool MasterPassword::validate(
    const std::string& password) {

    if (password.length() < 8)
        return false;

    bool upper = false;
    bool lower = false;
    bool digit = false;
    bool symbol = false;

    for (unsigned char ch : password) {
        if (ch >= 'A' && ch <= 'Z')
            upper = true;
        else if (ch >= 'a' && ch <= 'z')
            lower = true;
        else if (ch >= '0' && ch <= '9')
            digit = true;
        else
            symbol = true;
    }

    return upper && lower && digit && symbol;
}

bool MasterPassword::create(
    const std::string& password,
    const std::string& confirmation) {

    if (!validate(password))
        return false;

    return password == confirmation;
}