#include "TimetablePage.h"
#include "userinfo.h"
#include "teacherinfo.h"
#include <QVBoxLayout>
#include <QDebug>

TimetablePage::TimetablePage(QWidget *parent) : QWidget(parent) {
    timetable = new CustomTimetableWidget(this);
    switchButton = new QPushButton("切换到课程列表", this);
    switchButton->setText("⇄");
    switchButton->setFixedSize(36, 36);

    switchButton->setStyleSheet(R"(
    QPushButton {
        background-color: #a7e6a5;
        border: 2px solid #59c959;
        border-radius: 18px;
        font-size: 18px;
        font-weight: bold;
        color: #2d7035;
    }

    QPushButton:hover {
        background-color: #91db91;
        border-color: #4fb54f;
    }

    QPushButton:pressed {
        background-color: #7dcd7d;
    }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(switchButton);
    layout->addWidget(timetable);
    setLayout(layout);

    connect(switchButton, &QPushButton::clicked, this, [=]() {
        emit requestSwitchToList();
    });
}

void TimetablePage::setCourses(const QVector<CourseInfo> &courses,UserInfo *userinfo) {
    TeacherInfo *teacher1 = new TeacherInfo(this);
    if (userinfo == nullptr || teacher1->GetHasDoneLottery() == false) {
        timetable->setCourses(courses);
        return ;
    }
    QVector<CourseInfo> filtered;
    for (const auto &course : courses) {
        // qDebug() << course.code;
        if (userinfo->getResultForCourse(course.code) == true)
            filtered.append(course);
    }
    timetable->setCourses(filtered);
}

void TimetablePage::refreshCourses(UserInfo *user) {
    QVector<CourseInfo> courseList = user->getCurrentCourses();
    setCourses(courseList, user);
}
