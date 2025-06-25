#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTableWidget>
#include "CourseInfo.h"
#include "userinfo.h"
#include "coursecomment.h"
#include "timetablepage.h"
#include "courselistwidget.h"
#include "timetablepage.h"
#include "courseselection.h"

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

private:
    UserInfo* user;//这里是不是忘记改了？
    // QString currentUser;
    // QString currentRole;
    CourseListWidget *m_favoritePage;
    CourseSelection *m_selectionPage;
    QTableWidget *courseTable = nullptr;
    QListWidget *sidebar;
    QStackedWidget *mainStack;
    TimetablePage *timetablePage = nullptr;
    CourseListWidget *courseListPage = nullptr;
};
