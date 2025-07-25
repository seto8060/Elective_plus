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
#include <QJsonDocument>

class RegisterWindow : public QDialog {
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    QString getUsername() const;
    QString getPassword() const;
    // QString getRole() const;
    const UserInfo& getUserInfo() const;

private slots:
    void Register();

private:
    QLineEdit *Name_Box, *Pass_Box, *Confirm_Pass_Box, *Index_Box, *Real_name_Box;
    QComboBox *gradeBox, *collegeBox;
    QLabel *errorLabel;
    QPushButton *Register_Button;
    UserInfo newUser;
};

#endif // REGISTERWINDOW_H
