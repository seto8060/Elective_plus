#include "MainWindow.h"
#include "courseloader.h"
// #include "timetable.h"
#include "historypage.h"
#include "timetablepage.h"
#include "courselistwidget.h"
#include "courseselection.h"
#include "classquestionnaire.h"
#include "teacherinfo.h"
#include "homepage.h"
#include "commentloader.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QDebug>

MainWindow::MainWindow(UserInfo *userInfo,QWidget *parent) : QMainWindow(parent),user(userInfo) {
    All_courses = loadCoursesFromJsonFile("courses.json");
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
    sidebar->setIconSize(QSize(45, 45));
    QFont font;
    font.setPointSize(13);
    // font.setFamily("微软雅黑");
    sidebar->setFont(font);

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
    sidebar->setFixedWidth(220);
    TeacherInfo *teacher2 = new TeacherInfo(this);
    bool flag = teacher2->getEnrollmentTerm().semester > 0;
    if (!flag){
        item1->setFlags(item2->flags() & ~Qt::ItemIsEnabled);
        item1->setToolTip("现在不是选课时间");
        item2->setFlags(item2->flags() & ~Qt::ItemIsEnabled);
        item2->setToolTip("现在不是选课时间");
        item3->setFlags(item2->flags() & ~Qt::ItemIsEnabled);
        item3->setToolTip("现在不是选课时间");
    }

    mainStack = new QStackedWidget(this);
    mainStack->addWidget(new HomePage(userInfo,this));
    
    // 创建选课页面并连接信号
    CourseSelection *courseSelectionPage = new CourseSelection(All_courses, userInfo, allCoursesPtr,this);
    ClassQuestionnaire *ClassQuestionnairePage = new ClassQuestionnaire(userInfo, All_courses, All_comments, this);
    connect(ClassQuestionnairePage, &ClassQuestionnaire::favoritesUpdated, this, &MainWindow::updateFavoritesPage);
    // 连接收藏夹更新信号
    connect(courseSelectionPage, &CourseSelection::favoritesUpdated, this, &MainWindow::updateFavoritesPage);
    mainStack->addWidget(courseSelectionPage);
    qDebug()<<"CourseSelectionPage\n";
    mainStack->addWidget(ClassQuestionnairePage);
    
    // 保存收藏夹页面的指针，以便后续更新
    m_favoritePage = new CourseListWidget(this, 2, userInfo, "",allCoursesPtr);
    m_favoritePage->setCourses(userInfo->getFavorites(), 2, userInfo);
    mainStack->addWidget(m_favoritePage);
    qDebug()<<"favorpage\n";
    QVector<CourseInfo> courseList = userInfo->getCurrentCourses();


    //TimetablePage *timetablePage = new TimetablePage(this);
    //m_timetablePage=timetablePage;
    //CourseListWidget *courseListPage = new CourseListWidget(this,0,userInfo,"",allCoursesPtr);
    //m_courselist=courseListPage;
    timetablePage = new TimetablePage(this);
    courseListPage = new CourseListWidget(this,0,userInfo,"",allCoursesPtr);

    timetablePage->setCourses(courseList,user);
    courseListPage->setCourses(courseList,0,userInfo);

    mainStack->addWidget(timetablePage);    // index 5
    mainStack->addWidget(courseListPage);   // index 6

    //connect(courseSelectionPage,&CourseSelection::coursesUpdated,this,[courseList,timetablePage](){
    //    timetablePage->setCourses(courseList);
    //});

    connect(timetablePage, &TimetablePage::requestSwitchToList, this, [=]() {
        mainStack->setCurrentWidget(courseListPage);
    });

    connect(courseListPage, &CourseListWidget::requestSwitchToTable, this, [=]() {
        mainStack->setCurrentWidget(timetablePage);
    });


    HistoryPage* historyPage = new HistoryPage(userInfo, mainStack,allCoursesPtr);
    mainStack->addWidget(historyPage);

    auto *sidebarContainer = new QWidget(this);
    auto *sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(10);

    sidebarLayout->addWidget(sidebar);
    TeacherInfo *teacher3 = new TeacherInfo(this);
    QLabel *footerLabel = new QLabel("当前学期："+teacher3->getCurrentTerm().toString(), this);
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("font-size: 12px;font-weight: bold;");

    QLabel *footerLabel2 = new QLabel("当前选课学期："+teacher3->getEnrollmentTerm().toString(), this);
    footerLabel2->setAlignment(Qt::AlignCenter);
    footerLabel2->setStyleSheet("font-size: 12px;font-weight: bold;");


    QLabel *footerLabel3 = new QLabel("当前选课学期："+teacher3->GetHasDoneLottery()?"抽签已完成":"抽签未完成", this);
    footerLabel3->setAlignment(Qt::AlignCenter);
    footerLabel3->setStyleSheet("font-size: 12px;font-weight: bold;");

    sidebarLayout->addWidget(footerLabel);
    sidebarLayout->addWidget(footerLabel2);
    sidebarLayout->addWidget(footerLabel3);

    layout->addWidget(sidebarContainer);
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
    // qDebug() << index;
    // 当切换到收藏夹页面时，更新收藏夹内容
    if (index == 3) { // 收藏夹的索引是3
        updateFavoritesPage();
    }else if (index == 4) {
        timetablePage->refreshCourses(user);
        courseListPage->setCourses(user->getCurrentCourses(), 0, user);
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

//void MainWindow::updateCurrentCourse(){
//    if(m_courselist){
//        m_courselist->setCourses(user->getCurrentCourses(),0,user);
        //m_timetablePage->setCourses(user->getCurrentCourses());
//    }
//}

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

