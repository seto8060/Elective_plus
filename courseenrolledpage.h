#ifndef COURSEENROLLEDPAGE_H
#define COURSEENROLLEDPAGE_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include<QTableWidget>
#include "CourseInfo.h"
#include "UserInfo.h"

class courseEnrolledPage : public QWidget
{
    Q_OBJECT
public:
    explicit courseEnrolledPage(UserInfo* user, QWidget *parent = nullptr);
    //void addinCourseTable(CourseInfo& courses);
    void populateCourseTable(QVector<CourseInfo>& courses);

signals:
    void backRequested();
    void dropCourseRequested(const QString& courseCode);

private:
    void setupUI();

    QTableWidget *coursesTable;
};

#endif // COURSEENROLLEDPAGE_H
