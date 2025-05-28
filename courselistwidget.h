#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "CourseInfo.h"
#include "userinfo.h"

class CourseListWidget : public QWidget {
    Q_OBJECT
public:
    explicit CourseListWidget(QWidget *parent,int type,UserInfo *userinfo,const QString& semester);
    void setCourses(const QVector<CourseInfo> &courses,int type,UserInfo *userinfo);

signals:
    void requestSwitchToTable();

private:
    QTableWidget *table;
    QPushButton *switchButton;
    QLabel *footerNote;
};
