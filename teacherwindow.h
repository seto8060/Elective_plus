#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include <QLabel>
#include <QFile>
#include <QToolButton>
#include <QTableWidget>
#include <QJsonArray>
#include "teacherinfo.h"
#include "CourseInfo.h"

class TeacherWindow : public QWidget {
    Q_OBJECT

public:
    explicit TeacherWindow(QWidget *parent = nullptr);
    void refreshMainPage();
    bool checkConflict(const QString &userId, const QString &courseId) ;
    void importCoursesFromCSV();
    void refreshCourseTable();
    void saveCoursesToFile();
    void loadCoursesFromFile() {
        courses.clear();

        QFile file(currentCourseFilePath);
        if (!file.open(QIODevice::ReadOnly)) return;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (!doc.isArray()) return;

        for (const auto &v : doc.array()) {
            if (v.isObject())
                courses.append(parseCourseFromJson(v.toObject()));
        }
    }

private slots:
    void showMainPage();
    void showSubPage(const QString &pageName, const Term &operateTerm) ;

private:
    QWidget* createMainPage();
    QWidget* createSubPage(const QString &pageName, const Term &operateTerm);

    QStackedWidget *stackedWidget;
    QMap<QString, QWidget*> subPages;
    TeacherInfo *teacherInfo;
    QLabel *infoLabel = nullptr;
    QList<QToolButton*> functionButtons;
    QList<Term> functionTerms;
    QVector<CourseInfo> courses;
    QTableWidget *courseTable = nullptr;
    QString currentCourseFilePath;
};
/*
教务界面设计：
做成类似校内门户的界面
1.加/减课/修改选课信息
2.手工选课
3.开始/结束学期
4.抽签！
5.导出：每门课的选课信息、每门课的测评信息
可选功能：
批量操作用户
*/
