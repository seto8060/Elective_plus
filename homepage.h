#pragma once

#include <QWidget>
#include "userinfo.h"

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(UserInfo *user,QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};
