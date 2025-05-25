#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "CourseInfo.h"

class CourseListWidget : public QWidget {
    Q_OBJECT
public:
    explicit CourseListWidget(QWidget *parent = nullptr,int type = 0);
    void setCourses(const QVector<CourseInfo> &courses,int type);

signals:
    void requestSwitchToTable();

private:
    QTableWidget *table;
    QPushButton *switchButton;
};
