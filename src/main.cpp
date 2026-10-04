#include <QApplication>
#include "gui/LoginWindow.h"
#include "gui/MainWindow.h"

int main(int argc,char* argv[]){
    QApplication application(argc,argv);
    LoginWindow loginWindow;
    QObject::connect(&loginWindow,&LoginWindow::loginSuccessful,[&loginWindow](const QString& password){
        MainWindow* mainWindow=new MainWindow(password.toStdString());
        mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        loginWindow.hide();
        QObject::connect(mainWindow,&MainWindow::vaultLocked,[&loginWindow](){
            loginWindow.show();
            loginWindow.raise();
            loginWindow.activateWindow();
        });
        mainWindow->show();
    });
    loginWindow.show();
    return application.exec();
}