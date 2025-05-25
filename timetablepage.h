#pragma once
#include <QWidget>
#include <QPushButton>
#include "timetable.h"
#include "CourseInfo.h"

class TimetablePage : public QWidget {
    Q_OBJECT
public:
    explicit TimetablePage(QWidget *parent = nullptr);
    void setCourses(const QVector<CourseInfo> &courses);

signals:
    void requestSwitchToList();

private:
    CustomTimetableWidget *timetable;
    QPushButton *switchButton;
};
