#pragma once

#include <QWidget>
#include <QLabel>
#include <QVector>
#include "CourseInfo.h"

class CustomTimetableWidget : public QWidget {
    Q_OBJECT
public:
    explicit CustomTimetableWidget(QWidget *parent = nullptr);
    void setCourses(const QVector<CourseInfo> &courses);

private:
    QLabel *bgLabel;
    QWidget *overlayLayer;

    void setupBackground();
    void displayCourses(const QVector<CourseInfo> &courses);
    QPoint getCoursePosition(const QString &day, int section);
    int getDayColumn(const QString &day);
};
