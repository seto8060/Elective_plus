#include "courseselection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

CourseSelection::CourseSelection(const QVector<CourseInfo>& allCourses, UserInfo* user, QVector<courseComment>* courseComments,QWidget *parent)
    : QWidget(parent),  m_user(user),m_allCourses(allCourses),m_courseComments(courseComments)
{
    setupUI();
}

void CourseSelection::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 顶部按钮栏
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    // 创建两个功能按钮
    m_currentCourseButton = new QPushButton("已选课程", this);
    m_searchModButton = new QPushButton("课程检索", this);

    // 设置按钮样式
    QString buttonStyle = "QPushButton {"
                          "    padding: 10px 20px;"
                          "    font-size: 16px;"
                          "    border-radius: 5px;"
                          "    background-color: #4CAF50;"
                          "    color: white;"
                          "    border: none;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: #45a049;"
                          "}";

    m_currentCourseButton->setStyleSheet(buttonStyle);
    m_searchModButton->setStyleSheet(buttonStyle);

    // 将按钮添加到布局
    buttonLayout->addWidget(m_currentCourseButton);
    buttonLayout->addWidget(m_searchModButton);
    buttonLayout->addStretch(); // 添加伸缩项使按钮靠左

    // 创建堆栈窗口（虽然不实现具体页面，但仍保留结构）
    m_stackWidget = new QStackedWidget(this);

    //创建announcements页面
    QWidget *announcementPage = new QWidget();
    QVBoxLayout *announcementLayout = new QVBoxLayout(announcementPage);
    announcementLayout->addWidget(new QLabel("请点击上方按钮切换功能模块", announcementPage));
    announcementLayout->setAlignment(Qt::AlignCenter);

    // 创建课程搜索页面
    CourseSearchPage *m_searchPage=new CourseSearchPage(m_allCourses,m_user,m_courseComments,this);
    // 创建已选课程界面
    courseEnrolledPage *m_enrollPage=new courseEnrolledPage(m_user,this);

    // 将页面添加到堆栈
    m_stackWidget->addWidget(announcementPage);
    m_stackWidget->addWidget(m_searchPage);
    m_stackWidget->addWidget(m_enrollPage);
    //默认announcements
    m_stackWidget->setCurrentWidget(announcementPage);
    // 添加到主布局
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_stackWidget, 1); // 设置伸缩因子为1使表格区域可扩展

    // 连接信号和槽
    connect(m_enrollPage,&courseEnrolledPage::dropCourseRequested,this,[this,m_enrollPage](const QString& courseCode){
        auto& courses=m_user->getCurrentCourses();
        m_user->setPointForCourse(courseCode,0);
        courses.erase(std::remove_if(courses.begin(),courses.end(),[&courseCode](const CourseInfo& c){
            return c.code==courseCode;
        }),courses.end());

        m_enrollPage->populateCourseTable(m_user->getCurrentCourses());

        QMessageBox::information(this,"Success!",QString("成功退选%1").arg(courseCode));
    });
    connect(m_searchPage,&CourseSearchPage::coursesUpdated, m_enrollPage, [this, m_enrollPage](){
        m_enrollPage->populateCourseTable(m_user->getCurrentCourses());
    });
    connect(m_enrollPage,&courseEnrolledPage::backRequested,this,[this](){
        m_stackWidget->setCurrentIndex(0);
    });
    connect(m_searchModButton, &QPushButton::clicked, this, &CourseSelection::onSearchModButtonClicked);
    connect(m_currentCourseButton, &QPushButton::clicked, this, &CourseSelection::onCurrentCourseButtonClicked);
}

void CourseSelection::onSearchModButtonClicked()
{
    // 这里只发出信号，不实现具体页面
    m_stackWidget->setCurrentIndex(1);
    emit searchModRequested();
}

void CourseSelection::onCurrentCourseButtonClicked()
{
    // 这里只发出信号，不实现具体页面
    m_stackWidget->setCurrentIndex(2);
    emit currentCourseRequested();
}
