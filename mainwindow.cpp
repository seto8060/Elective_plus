#include "MainWindow.h"
#include "CourseLoader.h"
// #include "timetable.h"
#include "historypage.h"
#include "timetablepage.h"
#include "courselistwidget.h"
#include "courseSelection.h"
#include "homepage.h"
#include "commentloader.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QDebug>

MainWindow::MainWindow(UserInfo *userInfo,QWidget *parent) : QMainWindow(parent),user(userInfo) {
    All_courses = loadCoursesFromJsonFile(":/resources/resources/courses.json");
    All_comments=loadCommentsFromJsonFile("comments.json");
    QVector<courseComment> *allCoursesPtr=&All_comments;
    setWindowTitle("选课系统");
    resize(1000, 700);

    auto *central = new QWidget(this);
    auto *layout = new QHBoxLayout(central);

    sidebar = new QListWidget(this);
    // sidebar->setStyleSheet(R"(
    //     QListWidget {
    //         background-color: #1e1e1e;
    //         color: #f0f0f0;
    //         border: none;
    //         outline: none;
    //     }

    //     QListWidget::item {
    //         padding: 12px 16px;
    //         margin: 4px;
    //         border-radius: 6px;
    //     }

    //     QListWidget::item:selected {
    //         background-color: #0078d7;
    //         color: white;
    //         font-weight: bold;
    //     }

    //     QListWidget::item:hover {
    //         background-color: #3a3a3a;
    //     }
    // )");

    // sidebar->setFont(QFont("Segoe UI", 11));
    // sidebar->setFixedWidth(180);
    sidebar->setSpacing(20);
    sidebar->setIconSize(QSize(40, 40));

    // sidebar->addItem("选课主页");
    // sidebar->addItem("智能选课系统");
    // sidebar->addItem("收藏夹");
    // sidebar->addItem("选课列表");
    QListWidgetItem *item0 = new QListWidgetItem(QIcon(":/resources/icon/home.svg"), "主页");
    sidebar->addItem(item0);
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/resources/icon/list.svg"), "选课");
    sidebar->addItem(item1);
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/resources/icon/calculator.svg"), "智能选课系统");
    sidebar->addItem(item2);
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/resources/icon/heart.svg"), "收藏夹");
    sidebar->addItem(item3);
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/resources/icon/calendar.svg"), "选课列表");
    sidebar->addItem(item4);
    QListWidgetItem *hiddenItem = new QListWidgetItem("占位", sidebar);
    hiddenItem->setHidden(true);
    sidebar->addItem(hiddenItem);
    QListWidgetItem *item6 = new QListWidgetItem(QIcon(":/resources/icon/history.svg"), "历史选课");
    sidebar->addItem(item6);
    sidebar->setFixedWidth(150);

    mainStack = new QStackedWidget(this);
    mainStack->addWidget(new HomePage(userInfo,this));
    
    // 创建选课页面并连接信号
    CourseSelection *courseSelectionPage = new CourseSelection(All_courses, userInfo, allCoursesPtr,this);
    // 连接收藏夹更新信号
    connect(courseSelectionPage, &CourseSelection::favoritesUpdated, this, &MainWindow::updateFavoritesPage);
    mainStack->addWidget(courseSelectionPage);
    
    mainStack->addWidget(new QLabel("智能选课系统"));
    
    // 保存收藏夹页面的指针，以便后续更新
    m_favoritePage = new CourseListWidget(this, 2, userInfo, "",allCoursesPtr);
    m_favoritePage->setCourses(userInfo->getFavorites(), 2, userInfo);
    mainStack->addWidget(m_favoritePage);
    
    QVector<CourseInfo> courseList = userInfo->getCurrentCourses();


    TimetablePage *timetablePage = new TimetablePage(this);
    m_timetablePage=timetablePage;
    CourseListWidget *courseListPage = new CourseListWidget(this,0,userInfo,"",allCoursesPtr);
    m_courselist=courseListPage;

    timetablePage->setCourses(courseList);
    courseListPage->setCourses(courseList,0,userInfo);

    mainStack->addWidget(timetablePage);    // index 5
    mainStack->addWidget(courseListPage);   // index 6

    //connect(courseSelectionPage,&CourseSelection::coursesUpdated,this,[courseList,timetablePage](){
    //    timetablePage->setCourses(courseList);
    //});
    connect(courseSelectionPage,&CourseSelection::coursesUpdated,this,&MainWindow::updateCurrentCourse);

    connect(timetablePage, &TimetablePage::requestSwitchToList, this, [=]() {
        mainStack->setCurrentWidget(courseListPage);
    });

    connect(courseListPage, &CourseListWidget::requestSwitchToTable, this, [=]() {
        mainStack->setCurrentWidget(timetablePage);
    });


    HistoryPage* historyPage = new HistoryPage(userInfo, mainStack,allCoursesPtr);
    mainStack->addWidget(historyPage);

    layout->addWidget(sidebar);
    layout->addWidget(mainStack);
    //
    // courseTable = new QTableWidget(this);
    // courseTable->setColumnCount(5);
    // courseTable->setHorizontalHeaderLabels({"课程编号", "课程名称", "授课教师", "上课时间", "开课单位"});
    // courseTable->horizontalHeader()->setStretchLastSection(true);
    // courseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // loadCourseData();
    // layout->addWidget(courseTable);
    //
    setCentralWidget(central);

    connect(sidebar, &QListWidget::currentRowChanged, this, &MainWindow::changeModule);
    sidebar->setCurrentRow(0);  // 默认选中第一个模块
}

void MainWindow::changeModule(int index) {
    // 当切换到收藏夹页面时，更新收藏夹内容
    if (index == 3) { // 收藏夹的索引是3
        updateFavoritesPage();
    }
    if(index==0){
        updateCurrentCourse();

    }
    mainStack->setCurrentIndex(index);
}
void MainWindow::updateFavoritesPage(){
    if(m_favoritePage){
        m_favoritePage->setCourses(user->getFavorites(), 2, user);

    }
}
// void MainWindow::setUser(const QString &username, const QString &role) {
//     setWindowTitle(QString("%1%2您好，欢迎来到选课网++！").arg(user.getUsername(), user.getGrade()));
// }
void MainWindow::updateCurrentCourse(){
    qDebug() << "Updating timetable with courses:" << user->getCurrentCourses().size();
    if(m_courselist){
        m_courselist->setCourses(user->getCurrentCourses(),0,user);
        m_timetablePage->setCourses(user->getCurrentCourses());
    }
}

void MainWindow::loadCourseData() {
    // QVector<CourseInfo> All_courses = loadCoursesFromJsonFile(":/resources/resources/courses.json");
    // qDebug() << courses.size() << Qt::endl;
    courseTable->setRowCount(All_courses.size());

    for (int i = 0; i < All_courses.size(); ++i) {
        const CourseInfo &c = All_courses[i];
        courseTable->setItem(i, 0, new QTableWidgetItem(c.code));
        courseTable->setItem(i, 1, new QTableWidgetItem(c.name));
        courseTable->setItem(i, 2, new QTableWidgetItem(c.teacherList.join(", ")));
        courseTable->setItem(i, 3, new QTableWidgetItem(c.timeList.join("；")));
        courseTable->setItem(i, 4, new QTableWidgetItem(c.unit));
    }
}

