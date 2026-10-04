#ifndef MASTER_PASSWORD_H
#define MASTER_PASSWORD_H

#include <string>

class MasterPassword {
public:
    static bool validate(const std::string& password);

    static bool create(
        const std::string& password,
        const std::string& confirmation
    );
};

#endif