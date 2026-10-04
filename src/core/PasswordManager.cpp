#include "PasswordManager.h"
#include "../storage/VaultStorage.h"
#include <iostream>
#include <algorithm>

PasswordManager::PasswordManager()
    : nextId(1) {}

void PasswordManager::rebuildIndex() {
    idIndex.clear();

    for (size_t i = 0; i < entries.size(); ++i)
        idIndex[entries[i].getId()] = i;
}

int PasswordManager::addEntry(
    const std::string& website,
    const std::string& username,
    const std::string& password,
    const std::string& category,
    const std::string& notes) {

    PasswordEntry entry(
        nextId,
        website,
        username,
        password,
        category,
        notes
    );

    entries.push_back(entry);
    idIndex[nextId] = entries.size() - 1;

    return nextId++;
}

bool PasswordManager::deleteEntry(int id) {
    auto it = idIndex.find(id);

    if (it == idIndex.end())
        return false;

    entries.erase(entries.begin() + it->second);

    rebuildIndex();

    return true;
}

bool PasswordManager::updateEntry(
    int id,
    const std::string& website,
    const std::string& username,
    const std::string& password,
    const std::string& category,
    const std::string& notes) {

    PasswordEntry* entry = findById(id);

    if (entry == nullptr)
        return false;

    entry->setWebsite(website);
    entry->setUsername(username);
    entry->setPassword(password);
    entry->setCategory(category);
    entry->setNotes(notes);

    return true;
}

PasswordEntry* PasswordManager::findById(int id) {
    auto it = idIndex.find(id);

    if (it == idIndex.end())
        return nullptr;

    return &entries[it->second];
}

std::vector<PasswordEntry> PasswordManager::search(
    const std::string& query) const {

    std::vector<PasswordEntry> results;

    for (const auto& entry : entries) {
        if (entry.getWebsite().find(query) != std::string::npos ||
            entry.getUsername().find(query) != std::string::npos ||
            entry.getCategory().find(query) != std::string::npos ||
            entry.getNotes().find(query) != std::string::npos) {

            results.push_back(entry);
        }
    }

    return results;
}

void PasswordManager::sortByWebsite() {
    std::sort(
        entries.begin(),
        entries.end(),
        [](const PasswordEntry& a, const PasswordEntry& b) {
            return a.getWebsite() < b.getWebsite();
        }
    );

    rebuildIndex();
}

void PasswordManager::sortByUsername() {
    std::sort(
        entries.begin(),
        entries.end(),
        [](const PasswordEntry& a, const PasswordEntry& b) {
            return a.getUsername() < b.getUsername();
        }
    );

    rebuildIndex();
}

void PasswordManager::sortByCategory() {
    std::sort(
        entries.begin(),
        entries.end(),
        [](const PasswordEntry& a, const PasswordEntry& b) {
            return a.getCategory() < b.getCategory();
        }
    );

    rebuildIndex();
}

const std::vector<PasswordEntry>&
PasswordManager::getEntries() const {
    return entries;
}

void PasswordManager::displayEntries() const {
    if (entries.empty()) {
        std::cout << "No password entries found.\n";
        return;
    }

    for (const auto& entry : entries) {
        std::cout << "\nID: " << entry.getId()
                  << "\nWebsite: " << entry.getWebsite()
                  << "\nUsername: " << entry.getUsername()
                  << "\nPassword: ********"
                  << "\nCategory: " << entry.getCategory()
                  << "\nNotes: " << entry.getNotes()
                  << "\n-----------------------------\n";
    }
}

bool PasswordManager::saveToFile(
    const std::string& filename,
    const std::string& masterPassword) const {

    VaultStorage storage(filename);

    return storage.save(
        entries,
        masterPassword
    );
}

bool PasswordManager::loadFromFile(
    const std::string& filename,
    const std::string& masterPassword) {

    VaultStorage storage(filename);

    std::vector<PasswordEntry> loadedEntries;

    if (!storage.load(
            loadedEntries,
            masterPassword))
        return false;

    entries = loadedEntries;

    rebuildIndex();

    nextId = 1;

    for (const auto& entry : entries) {
        if (entry.getId() >= nextId)
            nextId = entry.getId() + 1;
    }

    return true;
}