#include "EntryDialog.h"
#include "../security/PasswordGenerator.h"
#include "../security/PasswordStrength.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

EntryDialog::EntryDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Password Entry");
    setFixedSize(500, 520);

    QLabel* title =
        new QLabel("Password Entry", this);

    title->setAlignment(Qt::AlignCenter);

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "margin-bottom: 10px;"
    );

    websiteEdit =
        new QLineEdit(this);

    websiteEdit->setPlaceholderText(
        "e.g. github.com"
    );

    usernameEdit =
        new QLineEdit(this);

    usernameEdit->setPlaceholderText(
        "Username or email"
    );

    passwordEdit =
        new QLineEdit(this);

    passwordEdit->setEchoMode(
        QLineEdit::Password
    );

    QPushButton* generateButton =
        new QPushButton(
            "Generate",
            this
        );

    QPushButton* showButton =
        new QPushButton(
            "Show",
            this
        );

    QHBoxLayout* passwordLayout =
        new QHBoxLayout();

    passwordLayout->addWidget(
        passwordEdit
    );

    passwordLayout->addWidget(
        showButton
    );

    passwordLayout->addWidget(
        generateButton
    );

    categoryBox =
        new QComboBox(this);

    categoryBox->addItems({
        "Personal",
        "Development",
        "Career",
        "Finance",
        "Social",
        "Shopping",
        "Other"
    });

    notesEdit =
        new QTextEdit(this);

    notesEdit->setPlaceholderText(
        "Additional information..."
    );

    notesEdit->setFixedHeight(80);

    strengthLabel =
        new QLabel(
            "Password Strength: Not analyzed",
            this
        );

    strengthLabel->setStyleSheet(
        "font-weight: bold;"
    );

    QFormLayout* form =
        new QFormLayout();

    form->addRow(
        "Website:",
        websiteEdit
    );

    form->addRow(
        "Username:",
        usernameEdit
    );

    form->addRow(
        "Password:",
        passwordLayout
    );

    form->addRow(
        "Category:",
        categoryBox
    );

    form->addRow(
        "Notes:",
        notesEdit
    );

    QPushButton* cancelButton =
        new QPushButton(
            "Cancel",
            this
        );

    QPushButton* saveButton =
        new QPushButton(
            "Save Entry",
            this
        );

    QHBoxLayout* buttonLayout =
        new QHBoxLayout();

    buttonLayout->addStretch();

    buttonLayout->addWidget(
        cancelButton
    );

    buttonLayout->addWidget(
        saveButton
    );

    QVBoxLayout* layout =
        new QVBoxLayout(this);

    layout->addWidget(title);
    layout->addLayout(form);
    layout->addWidget(strengthLabel);
    layout->addStretch();
    layout->addLayout(buttonLayout);

    setLayout(layout);

    connect(
        generateButton,
        &QPushButton::clicked,
        this,
        &EntryDialog::generatePassword
    );

    connect(
        showButton,
        &QPushButton::clicked,
        this,
        [this, showButton]() {

            if (passwordEdit->echoMode() ==
                QLineEdit::Password) {

                passwordEdit->setEchoMode(
                    QLineEdit::Normal
                );

                showButton->setText("Hide");
            }
            else {

                passwordEdit->setEchoMode(
                    QLineEdit::Password
                );

                showButton->setText("Show");
            }
        }
    );

    connect(
        passwordEdit,
        &QLineEdit::textChanged,
        this,
        &EntryDialog::updateStrength
    );

    connect(
        cancelButton,
        &QPushButton::clicked,
        this,
        &EntryDialog::reject
    );

    connect(
        saveButton,
        &QPushButton::clicked,
        this,
        &EntryDialog::saveEntry
    );
}

void EntryDialog::setEntry(
    const std::string& website,
    const std::string& username,
    const std::string& password,
    const std::string& category,
    const std::string& notes) {

    websiteEdit->setText(
        QString::fromStdString(website)
    );

    usernameEdit->setText(
        QString::fromStdString(username)
    );

    passwordEdit->setText(
        QString::fromStdString(password)
    );

    int categoryIndex =
        categoryBox->findText(
            QString::fromStdString(category)
        );

    if (categoryIndex >= 0)
        categoryBox->setCurrentIndex(categoryIndex);

    notesEdit->setPlainText(
        QString::fromStdString(notes)
    );

    updateStrength();
}

void EntryDialog::generatePassword() {

    try {

        std::string password =
            PasswordGenerator::generate(
                16,
                true,
                true,
                true,
                true
            );

        passwordEdit->setText(
            QString::fromStdString(password)
        );

    }
    catch (...) {

        QMessageBox::warning(
            this,
            "Error",
            "Unable to generate password."
        );
    }
}

void EntryDialog::updateStrength() {

    std::string password =
        passwordEdit->text().toStdString();

    if (password.empty()) {

        strengthLabel->setText(
            "Password Strength: Not analyzed"
        );

        return;
    }

    PasswordStrengthResult result =
        PasswordStrength::analyze(
            password
        );

    strengthLabel->setText(
        QString(
            "Password Strength: %1 (%2/10)"
        )
        .arg(
            QString::fromStdString(
                result.level
            )
        )
        .arg(result.score)
    );
}

void EntryDialog::saveEntry() {

    if (websiteEdit->text().isEmpty()) {

        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter a website."
        );

        return;
    }

    if (usernameEdit->text().isEmpty()) {

        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter a username."
        );

        return;
    }

    if (passwordEdit->text().isEmpty()) {

        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter a password."
        );

        return;
    }

    accept();
}

std::string EntryDialog::getWebsite() const {

    return websiteEdit->
        text().
        toStdString();
}

std::string EntryDialog::getUsername() const {

    return usernameEdit->
        text().
        toStdString();
}

std::string EntryDialog::getPassword() const {

    return passwordEdit->
        text().
        toStdString();
}

std::string EntryDialog::getCategory() const {

    return categoryBox->
        currentText().
        toStdString();
}

std::string EntryDialog::getNotes() const {

    return notesEdit->
        toPlainText().
        toStdString();
}