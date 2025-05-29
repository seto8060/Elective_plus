#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "userinfo.h"

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccess(UserInfo *user);

private slots:
    void handleLogin();
    void handleRegister();

private:
    QLineEdit *Name_Box;
    QLineEdit *Pass_Box, *verifyBox;
    QPushButton *Display_Pass;
    QComboBox *roleBox;
    QPushButton *Login_Button, *Register_Button;
    QLabel *errorLabel, *verifyLabel;

    bool validateCredentials(const QString &user, const QString &pass, const QString &role);
};
void saveUserToJson(UserInfo *user) ;
#endif // LOGINWINDOW_H
