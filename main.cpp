#include <QApplication>
#include<QDebug>
#include "LoginWindow.h"
#include "MainWindow.h"
#include "teacherwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginWindow login;
    // MainWindow mainWin();

    QObject::connect(&login, &LoginWindow::loginSuccess, [&](UserInfo *user) {
        // qDebug() << user ;
        if (!user->IsTeacher){
            MainWindow *mainWin = new MainWindow(user);
            mainWin->setAttribute(Qt::WA_DeleteOnClose);
            mainWin->show();

            QObject::connect(mainWin, &QObject::destroyed, [=]() {
                saveUserToJson(user);
                qDebug() << "saved" << user->getUsername() << Qt::endl;
                delete user;
            });
        }
        else {
            TeacherWindow *mainWin = new TeacherWindow(user);
            mainWin->setAttribute(Qt::WA_DeleteOnClose);
            mainWin->show();

            QObject::connect(mainWin, &QObject::destroyed, [=]() {
                saveUserToJson(user);
                qDebug() << "saved teacher " << user->getUsername() << Qt::endl;
                delete user;
            });
        }
    });

    login.show();

    return app.exec();
}
