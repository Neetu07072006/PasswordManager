#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QMainWindow>
#include "../core/PasswordManager.h"
class QTableWidget;
class QLineEdit;
class QTimer;
class MainWindow:public QMainWindow{
    Q_OBJECT
private:
    PasswordManager manager;
    std::string masterPassword;
    QTableWidget* table;
    QLineEdit* searchBox;
    QTimer* inactivityTimer;
    void clearSensitiveData();
    void loadEntries();
    void searchEntries();
    void addEntry();
    void editEntry();
    void viewPassword();
    void deleteEntry();
    void lockVault();
    void resetInactivityTimer();
protected:
    bool eventFilter(QObject* watched,QEvent* event) override;
signals:
    void vaultLocked();
public:
    MainWindow(const std::string& masterPassword,QWidget* parent=nullptr);
    ~MainWindow();
};
#endif