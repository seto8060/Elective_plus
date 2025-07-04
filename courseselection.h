#ifndef COURSESELECTION_H
#define COURSESELECTION_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include "CourseInfo.h"
#include "CourseSearchPage.h"
#include "CourseEnrolledPage.h"
#include "UserInfo.h"

class CourseSelection : public QWidget {
    Q_OBJECT
public:
    explicit CourseSelection(const QVector<CourseInfo>& allCourses, UserInfo* user,QVector<courseComment>* courseComments, QWidget *parent = nullptr);

signals:
    void favoritesUpdated();
    void searchModRequested();
    void currentCourseRequested();
    void coursesUpdated();

private:
    void setupUI();

private slots:
    void onSearchModButtonClicked();
    void onCurrentCourseButtonClicked();


private:
    UserInfo* m_user;
    QVector<CourseInfo> m_allCourses;
    QVector<courseComment>* m_courseComments;
    QPushButton *m_searchModButton;
    QPushButton *m_currentCourseButton;
    QStackedWidget *m_stackWidget;
    CourseSearchPage* toolToUpdate;
public:
    courseEnrolledPage* mm_enrollPage;
};

#endif // COURSESELECTION_H
