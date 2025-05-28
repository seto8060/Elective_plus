#include "CourseListWidget.h"
#include "historypage.h"
#include "timetablepage.h"
#include "userinfo.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QDebug>

HistoryPage::HistoryPage(UserInfo* userInfo, QStackedWidget* mainStack, QWidget* parent)
        : QWidget(parent), user(userInfo), stack(mainStack) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("历史学期", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(title);

    QScrollArea* scrollArea = new QScrollArea(this);
    QWidget* scrollWidget = new QWidget(this);
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(5);
    scrollLayout->setAlignment(Qt::AlignHCenter);
    scrollLayout->addStretch();
    for (const auto& pair : user->getPastSemestersCourses()) {
        QString term = pair.first;
        qDebug() << term ;
        QVector<CourseInfo> courses = pair.second;

        QPushButton* btn = new QPushButton(term, this);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #eef;
                border: 1px solid #88a;
                border-radius: 6px;
                font-size: 16px;
                padding: 6px;
            }
            QPushButton:hover {
                background-color: #dde;
            }
        )");
        btn->setFixedSize(500,50);

        QHBoxLayout* hLayout = new QHBoxLayout;
        hLayout->addStretch();
        hLayout->addWidget(btn);
        hLayout->addStretch();

        QWidget* container = new QWidget(this);
        container->setLayout(hLayout);

        scrollLayout->addWidget(container);

        connect(btn, &QPushButton::clicked, this, [=]() {
            // qDebug() << 1;
            // TimetablePage *timetablePage = new TimetablePage(this);
            // CourseListWidget *courseListPage = new CourseListWidget(this,0,userInfo);

            // timetablePage->setCourses(courseList);
            // courseListPage->setCourses(courseList,0,userInfo);

            // mainStack->addWidget(timetablePage);    // index 5
            // mainStack->addWidget(courseListPage);   // index 6


            auto* historytimetable = new TimetablePage(this);
            auto* historyList = new CourseListWidget(this, 1, user,term);
            qDebug() << 1 ;
            historytimetable->setCourses(courses);
            historyList->setCourses(courses, 1, user);
            stack->addWidget(historytimetable);
            stack->addWidget(historyList);
            stack->setCurrentWidget(historyList);
            connect(historytimetable, &TimetablePage::requestSwitchToList, this, [=]() {
                stack->setCurrentWidget(historyList);
            });

            connect(historyList, &CourseListWidget::requestSwitchToTable, this, [=]() {
                stack->setCurrentWidget(historytimetable);
            });
        });

    }
    scrollLayout->addStretch();
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);
}


