#ifndef VAULT_STORAGE_H
#define VAULT_STORAGE_H
#include "../core/PasswordEntry.h"
#include <string>
#include <vector>
class VaultStorage{
private:
    std::string filename;
    std::string serialize(const std::vector<PasswordEntry>& entries) const;
    std::vector<PasswordEntry> deserialize(const std::string& data) const;
public:
    explicit VaultStorage(const std::string& filename);
    bool save(const std::vector<PasswordEntry>& entries,const std::string& masterPassword) const;
    bool load(std::vector<PasswordEntry>& entries,const std::string& masterPassword) const;
    bool exists() const;
    std::string getFilename() const;
};
#endif