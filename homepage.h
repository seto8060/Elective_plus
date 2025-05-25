#pragma once

#include <QWidget>
#include "userinfo.h"

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(const UserInfo &user,QWidget *parent = nullptr);
};
