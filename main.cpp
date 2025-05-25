#include <QApplication>
#include "LoginWindow.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginWindow login;
    // MainWindow mainWin();

    QObject::connect(&login, &LoginWindow::loginSuccess, [&](const UserInfo &user) {
        MainWindow *mainWin = new MainWindow(user);
        mainWin->show();
    });

    login.show();

    return app.exec();
}
