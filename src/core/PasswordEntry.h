#ifndef PASSWORD_ENTRY_H
#define PASSWORD_ENTRY_H

#include <string>

class PasswordEntry {
private:
    int id;
    std::string website;
    std::string username;
    std::string password;
    std::string category;
    std::string notes;

public:
    PasswordEntry();
    PasswordEntry(int id, const std::string& website, const std::string& username,
                  const std::string& password, const std::string& category,
                  const std::string& notes);

    int getId() const;
    std::string getWebsite() const;
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getCategory() const;
    std::string getNotes() const;

    void setWebsite(const std::string& website);
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void setCategory(const std::string& category);
    void setNotes(const std::string& notes);
};

#endif