#include "LoginWindow.h"
#include "../core/PasswordManager.h"
#include "../security/MasterPassword.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget* parent):QWidget(parent){
    setWindowTitle("Password Manager");
    setFixedSize(420,360);
    vaultExists=QFileInfo::exists("vault.dat");
    titleLabel=new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size:26px;font-weight:bold;");
    subtitleLabel=new QLabel(this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    passwordEdit=new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmationEdit=new QLineEdit(this);
    confirmationEdit->setEchoMode(QLineEdit::Password);
    actionButton=new QPushButton(this);
    statusLabel=new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    QVBoxLayout* layout=new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addSpacing(15);
    layout->addWidget(passwordEdit);
    layout->addWidget(confirmationEdit);
    layout->addWidget(actionButton);
    layout->addWidget(statusLabel);
    layout->addStretch();
    setLayout(layout);
    connect(actionButton,&QPushButton::clicked,this,[this](){
        if(vaultExists)
            unlockVault();
        else
            createVault();
    });
    connect(passwordEdit,&QLineEdit::returnPressed,this,[this](){
        if(vaultExists)
            unlockVault();
        else
            createVault();
    });
    connect(confirmationEdit,&QLineEdit::returnPressed,this,[this](){
        if(!vaultExists)
            createVault();
    });
    updateMode();
}

void LoginWindow::updateMode(){
    if(vaultExists){
        titleLabel->setText("Password Manager");
        subtitleLabel->setText("Enter your master password to unlock the vault.");
        passwordEdit->setPlaceholderText("Master password");
        confirmationEdit->hide();
        actionButton->setText("Unlock Vault");
    }else{
        titleLabel->setText("Create Password Vault");
        subtitleLabel->setText("Create a strong master password to protect your encrypted vault.");
        passwordEdit->setPlaceholderText("Master password");
        confirmationEdit->setPlaceholderText("Confirm master password");
        confirmationEdit->show();
        actionButton->setText("Create Vault");
    }
    statusLabel->clear();
}

void LoginWindow::unlockVault(){
    QString password=passwordEdit->text();
    if(password.isEmpty()){
        statusLabel->setText("Please enter your master password.");
        return;
    }
    PasswordManager manager;
    if(!manager.loadFromFile("vault.dat",password.toStdString())){
        statusLabel->setText("Incorrect master password or invalid vault.");
        passwordEdit->clear();
        passwordEdit->setFocus();
        return;
    }
    passwordEdit->clear();
    confirmationEdit->clear();
    statusLabel->clear();
    emit loginSuccessful(password);
}

void LoginWindow::createVault(){
    QString password=passwordEdit->text();
    QString confirmation=confirmationEdit->text();
    if(!MasterPassword::validate(password.toStdString())){
        statusLabel->setText("Password must contain at least 8 characters, including uppercase, lowercase, number and symbol.");
        return;
    }
    if(password!=confirmation){
        statusLabel->setText("Master passwords do not match.");
        confirmationEdit->clear();
        confirmationEdit->setFocus();
        return;
    }
    PasswordManager manager;
    if(!manager.saveToFile("vault.dat",password.toStdString())){
        passwordEdit->clear();
        confirmationEdit->clear();
        QMessageBox::critical(this,"Vault Creation Failed","The encrypted vault could not be created.");
        return;
    }
    passwordEdit->clear();
    confirmationEdit->clear();
    vaultExists=true;
    updateMode();
    QMessageBox::information(this,"Vault Created","Your encrypted password vault has been created successfully.");
}