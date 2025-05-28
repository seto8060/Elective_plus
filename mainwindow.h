#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTableWidget>
#include "CourseInfo.h"
#include "userinfo.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(UserInfo *userInfo, QWidget *parent = nullptr);
    void setUser(const QString &username, const QString &role);
    void loadCourseData();
    QVector<CourseInfo> All_courses;

private slots:
    void changeModule(int index);

private:
    UserInfo user;
    // QString currentUser;
    // QString currentRole;

    QTableWidget *courseTable = nullptr;
    QListWidget *sidebar;
    QStackedWidget *mainStack;
};
