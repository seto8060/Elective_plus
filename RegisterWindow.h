#pragma once

#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H
#include<QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "userinfo.h"

class RegisterWindow : public QDialog {
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    QString getUsername() const;
    QString getPassword() const;
    QString getRole() const;
    const UserInfo& getUserInfo() const;

private slots:
    void Register();

private:
    QLineEdit *Name_Box, *Pass_Box, *Confirm_Pass_Box;
    QComboBox *rolebox, *gradeBox, *collegeBox;
    QLabel *errorLabel;
    QPushButton *Register_Button;
    UserInfo newUser;
};

#endif // REGISTERWINDOW_H
