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
    explicit CourseSelection(const QVector<CourseInfo>& allCourses, const UserInfo& user, QWidget *parent = nullptr);

signals:
    void searchModRequested();
    void currentCourseRequested();

private:
    void setupUI();

private slots:
    void onSearchModButtonClicked();
    void onCurrentCourseButtonClicked();

private:
    UserInfo m_user;
    QVector<CourseInfo> m_allCourses;
    QPushButton *m_searchModButton;
    QPushButton *m_currentCourseButton;
    QStackedWidget *m_stackWidget;
};

#endif // COURSESELECTION_H
