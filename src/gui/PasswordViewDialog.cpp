#include "PasswordViewDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QTimer>

PasswordViewDialog::PasswordViewDialog(const std::string& website,const std::string& username,const std::string& password,QWidget* parent):QDialog(parent){
    setWindowTitle("Stored Password");
    setFixedSize(450,280);
    QLabel* title=new QLabel("Stored Password",this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:22px;font-weight:bold;");
    QLabel* websiteLabel=new QLabel(QString::fromStdString(website),this);
    QLabel* usernameLabel=new QLabel(QString::fromStdString(username),this);
    passwordEdit=new QLineEdit(QString::fromStdString(password),this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    QPushButton* showButton=new QPushButton("Show Password",this);
    QPushButton* copyButton=new QPushButton("Copy Password",this);
    QPushButton* closeButton=new QPushButton("Close",this);
    QLabel* statusLabel=new QLabel("Clipboard is cleared automatically after 15 seconds.",this);
    statusLabel->setWordWrap(true);
    QFormLayout* form=new QFormLayout();
    form->addRow("Website:",websiteLabel);
    form->addRow("Username:",usernameLabel);
    form->addRow("Password:",passwordEdit);
    QHBoxLayout* buttons=new QHBoxLayout();
    buttons->addStretch();
    buttons->addWidget(showButton);
    buttons->addWidget(copyButton);
    buttons->addWidget(closeButton);
    QVBoxLayout* layout=new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addLayout(form);
    layout->addWidget(statusLabel);
    layout->addStretch();
    layout->addLayout(buttons);
    connect(showButton,&QPushButton::clicked,this,[this,showButton](){
        if(passwordEdit->echoMode()==QLineEdit::Password){
            passwordEdit->setEchoMode(QLineEdit::Normal);
            showButton->setText("Hide Password");
        }else{
            passwordEdit->setEchoMode(QLineEdit::Password);
            showButton->setText("Show Password");
        }
    });
    connect(copyButton,&QPushButton::clicked,this,&PasswordViewDialog::copyPassword);
    connect(closeButton,&QPushButton::clicked,this,&PasswordViewDialog::accept);
}

void PasswordViewDialog::copyPassword(){
    QString password=passwordEdit->text();
    QClipboard* clipboard=QApplication::clipboard();
    if(!clipboard)
        return;
    clipboard->setText(password);
    QTimer::singleShot(15000,qApp,[password](){
        QClipboard* clipboard=QApplication::clipboard();
        if(clipboard&&clipboard->text()==password)
            clipboard->clear();
    });
}