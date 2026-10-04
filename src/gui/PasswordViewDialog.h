#ifndef PASSWORD_VIEW_DIALOG_H
#define PASSWORD_VIEW_DIALOG_H
#include <QDialog>
#include <string>
class QLineEdit;
class PasswordViewDialog:public QDialog{
    Q_OBJECT
private:
    QLineEdit* passwordEdit;
    void copyPassword();
public:
    PasswordViewDialog(const std::string& website,const std::string& username,const std::string& password,QWidget* parent=nullptr);
};
#endif