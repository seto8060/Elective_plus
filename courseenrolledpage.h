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
    explicit courseEnrolledPage(UserInfo& user, QWidget *parent = nullptr);

signals:
    void backRequested();
    void dropCourseRequested(const QString& courseCode);

private:
    void setupUI();
    void populateCourseTable(QVector<CourseInfo>& courses);
    QTableWidget *coursesTable;
};

#endif // COURSEENROLLEDPAGE_H
