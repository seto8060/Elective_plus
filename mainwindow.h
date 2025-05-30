#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTableWidget>
#include "CourseInfo.h"
#include "userinfo.h"
#include "coursecomment.h"
#include "courselistwidget.h"
#include "timetablepage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(UserInfo *userInfo, QWidget *parent = nullptr);
    void setUser(const QString &username, const QString &role);
    void loadCourseData();
    QVector<CourseInfo> All_courses;
    QVector<courseComment> All_comments;

private slots:
    void changeModule(int index);

public slots:
    void updateFavoritesPage();
    void updateCurrentCourse();

private:
    UserInfo* user;//这里是不是忘记改了？
    // QString currentUser;
    // QString currentRole;
    CourseListWidget *m_favoritePage;
    CourseListWidget *m_courselist;
    QTableWidget *courseTable = nullptr;
    TimetablePage *m_timetablePage;
    QListWidget *sidebar;
    QStackedWidget *mainStack;
};
