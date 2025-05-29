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


class QTableWidget;
class QPushButton;
class QLineEdit;
class QStackedWidget;

class CourseSearchPage : public QWidget {
    Q_OBJECT
public:
    explicit CourseSearchPage(const QVector<CourseInfo>& allCourses, QWidget *parent = nullptr);

private:
    void setupUI();
    void populateCourseTable(const QVector<CourseInfo>& courses);

private slots:
    void onSearchButtonClicked();
    void onAddToFavoritesClicked();
    void onEnrollCourseClicked();

private:
    QVector<CourseInfo> m_allCourses;
    QLineEdit *m_searchLineEdit;
    QPushButton *m_searchButton;
    QTableWidget *m_courseTable;
    QPushButton *m_addToFavoritesButton;
    QPushButton *m_enrollButton;
    QStackedWidget *m_stackWidget;  // 添加这个成员变量
};

#endif //COURSESEARCHPAGE_H
