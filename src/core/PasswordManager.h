#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include "PasswordEntry.h"
#include <vector>
#include <unordered_map>
#include <string>

class PasswordManager {
private:
    std::vector<PasswordEntry> entries;
    std::unordered_map<int, size_t> idIndex;
    int nextId;

    void rebuildIndex();

public:
    PasswordManager();

    int addEntry(
        const std::string& website,
        const std::string& username,
        const std::string& password,
        const std::string& category,
        const std::string& notes
    );

    bool deleteEntry(int id);

    bool updateEntry(
        int id,
        const std::string& website,
        const std::string& username,
        const std::string& password,
        const std::string& category,
        const std::string& notes
    );

    PasswordEntry* findById(int id);

    std::vector<PasswordEntry> search(
        const std::string& query) const;

    void sortByWebsite();
    void sortByUsername();
    void sortByCategory();

    const std::vector<PasswordEntry>& getEntries() const;

    void displayEntries() const;

    bool saveToFile(
        const std::string& filename,
        const std::string& masterPassword) const;

    bool loadFromFile(
        const std::string& filename,
        const std::string& masterPassword);
};

#endif