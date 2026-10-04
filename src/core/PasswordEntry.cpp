#include "PasswordEntry.h"

PasswordEntry::PasswordEntry()
    : id(0) {}

PasswordEntry::PasswordEntry(int id, const std::string& website,
                             const std::string& username,
                             const std::string& password,
                             const std::string& category,
                             const std::string& notes)
    : id(id), website(website), username(username),
      password(password), category(category), notes(notes) {}

int PasswordEntry::getId() const {
    return id;
}

std::string PasswordEntry::getWebsite() const {
    return website;
}

std::string PasswordEntry::getUsername() const {
    return username;
}

std::string PasswordEntry::getPassword() const {
    return password;
}

std::string PasswordEntry::getCategory() const {
    return category;
}

std::string PasswordEntry::getNotes() const {
    return notes;
}

void PasswordEntry::setWebsite(const std::string& website) {
    this->website = website;
}

void PasswordEntry::setUsername(const std::string& username) {
    this->username = username;
}

void PasswordEntry::setPassword(const std::string& password) {
    this->password = password;
}

void PasswordEntry::setCategory(const std::string& category) {
    this->category = category;
}

void PasswordEntry::setNotes(const std::string& notes) {
    this->notes = notes;
}