#ifndef ENTRY_DIALOG_H
#define ENTRY_DIALOG_H

#include <QDialog>
#include <string>

class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;
class QPushButton;

class EntryDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* websiteEdit;
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QComboBox* categoryBox;
    QTextEdit* notesEdit;
    QLabel* strengthLabel;

    void generatePassword();
    void updateStrength();
    void saveEntry();

public:
    explicit EntryDialog(QWidget* parent = nullptr);

    void setEntry(
        const std::string& website,
        const std::string& username,
        const std::string& password,
        const std::string& category,
        const std::string& notes
    );

    std::string getWebsite() const;
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getCategory() const;
    std::string getNotes() const;
};

#endif