#ifndef COURSESEARCHPAGE_H
#define COURSESEARCHPAGE_H

#include <QWidget>
#include <QVector>
#include<QLineEdit>
#include<QPushButton>
#include<QTableWidget>
#include<QStackedWidget>
#include"coursedetailpage.h"
#include "CourseInfo.h"
#include "UserInfo.h"
#include <QTimer>
#include<QMessageBox>


class QTableWidget;
class QPushButton;
class QLineEdit;
class QStackedWidget;

class CourseSearchPage : public QWidget {
    Q_OBJECT
public:
    explicit CourseSearchPage(const QVector<CourseInfo>& allCourses, UserInfo* user,QVector<courseComment>* courseComments,QWidget *parent = nullptr);

private:
    void setupUI();
    void populateCourseTable(const QVector<CourseInfo>& courses);
    void handleElectCourse(const CourseInfo& course);
    void setRatingDisplay(QTableWidgetItem *item, double total,double hw,double exam, double listen,int cnt);

signals:
    void favoritesUpdated();
    void coursesUpdated();
private slots:
    void onSearchButtonClicked();


private:
    bool m_isHandlingEnroll=false;
    UserInfo* m_user;
    QVector<CourseInfo> m_allCourses;
    // 修改为指针传递
    QVector<courseComment>* m_courseComments;
    QLineEdit *m_searchLineEdit;
    QPushButton *m_searchButton;
    QTableWidget *m_courseTable;
    //QPushButton *m_addToFavoritesButton;
    //QPushButton *m_enrollButton;
    QStackedWidget *m_stackWidget;  // 添加这个成员变量
};

#endif //COURSESEARCHPAGE_H
