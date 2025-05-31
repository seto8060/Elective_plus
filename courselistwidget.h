#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "CourseInfo.h"
#include "userinfo.h"
#include "coursecomment.h"
#include "courseloader.h"
#include "term.h"

class CourseListWidget : public QWidget {
    Q_OBJECT
public:
    explicit CourseListWidget(QWidget *parent,int type,UserInfo *userinfo,const QString& semester,QVector<courseComment> *allcoursecomments);
    void setCourses(const QVector<CourseInfo> &courses,int type,UserInfo *userinfo);

signals:
    void requestSwitchToTable();
    void commentUpdated();

private:
    QTableWidget *table;
    QPushButton *switchButton;
    QLabel *footerNote;
    QVector<courseComment> *m_allCourseComments;
    QString semester;
};
