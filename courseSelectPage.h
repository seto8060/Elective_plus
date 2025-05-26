// CourseSelectionPage.h
#ifndef COURSESELECTIONPAGE_H
#define COURSESELECTIONPAGE_H

#include <QWidget>
#include <QVector>
#include<QLineEdit>
#include<QPushButton>
#include<QTableWidget>
#include<QStackedWidget>  // 添加这个头文件
#include"coursedetailpage.h"
#include "CourseInfo.h"


class QTableWidget;
class QPushButton;
class QLineEdit;
class QStackedWidget;

class CourseSelectionPage : public QWidget {
    Q_OBJECT
public:
    explicit CourseSelectionPage(const QVector<CourseInfo>& allCourses, QWidget *parent = nullptr);

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

#endif // COURSESELECTIONPAGE_H
