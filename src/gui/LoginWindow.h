#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H
#include <QWidget>
class QLineEdit;
class QPushButton;
class QLabel;
class LoginWindow:public QWidget{
    Q_OBJECT
private:
    QLineEdit* passwordEdit;
    QLineEdit* confirmationEdit;
    QPushButton* actionButton;
    QLabel* titleLabel;
    QLabel* subtitleLabel;
    QLabel* statusLabel;
    bool vaultExists;
    void updateMode();
    void unlockVault();
    void createVault();
signals:
    void loginSuccessful(const QString& password);
public:
    explicit LoginWindow(QWidget* parent=nullptr);
};
#endif