#include "coursesearchpage.h"
#include "coursedetailpage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStackedWidget>  // 确保包含这个头文件
#include <QDebug>
#include<QInputDialog>

CourseSearchPage::CourseSearchPage(const QVector<CourseInfo>& allCourses, UserInfo* user,const QVector<courseComment>& courseComments,QWidget *parent)
    : QWidget(parent),m_user(user) , m_allCourses(allCourses),m_courseComments(courseComments) {
    setupUI();
    populateCourseTable(allCourses);
}

void CourseSearchPage::setupUI() {
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建堆叠窗口部件
    m_stackWidget = new QStackedWidget(this);
    
    // 创建主内容窗口部件
    QWidget *mainContentWidget = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(mainContentWidget);

    // 搜索栏
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("搜索课程名称、教师或编号...");
    m_searchButton = new QPushButton("搜索", this);
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(m_searchButton);

    // 课程表格
    m_courseTable = new QTableWidget(this);
    m_courseTable->setColumnCount(6); // 编号、名称、教师、时间、单位、操作
    m_courseTable->setHorizontalHeaderLabels({"课程编号", "课程名称", "授课教师", "上课时间", "开课单位", "操作"});
    m_courseTable->horizontalHeader()->setStretchLastSection(true);
    m_courseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_courseTable->setSelectionBehavior(QAbstractItemView::SelectRows);


    // 添加到内容布局
    contentLayout->addLayout(searchLayout);
    contentLayout->addWidget(m_courseTable);

    
    // 将主内容窗口添加到堆叠窗口
    m_stackWidget->addWidget(mainContentWidget);
    
    // 将堆叠窗口添加到主布局
    mainLayout->addWidget(m_stackWidget);

    // 连接信号槽
    connect(m_searchButton, &QPushButton::clicked, this, &CourseSearchPage::onSearchButtonClicked);
    

}

void CourseSearchPage::populateCourseTable(const QVector<CourseInfo>& courses) {
    m_courseTable->setRowCount(courses.size());

    for (int i = 0; i < courses.size(); ++i) {
        const CourseInfo &c = courses[i];
        m_courseTable->setItem(i, 0, new QTableWidgetItem(c.code));
        m_courseTable->setItem(i, 1, new QTableWidgetItem(c.name));
        m_courseTable->setItem(i, 2, new QTableWidgetItem(c.teacherList.join(", ")));
        m_courseTable->setItem(i, 3, new QTableWidgetItem(c.timeList.join("；")));
        m_courseTable->setItem(i, 4, new QTableWidgetItem(c.unit));

        courseComment tar;
        for(const auto& v:m_courseComments){
            if(v.code==c.code){
                tar=v;
            }
        }
        // 添加操作按钮
        QPushButton *detailButton = new QPushButton("查看详情", this);
        connect(detailButton, &QPushButton::clicked, [this, tar,c]() {
            CourseDetailPage *detailPage = new CourseDetailPage(c, tar,this);
            connect(detailPage, &CourseDetailPage::backRequested, [this, detailPage]() {
                m_stackWidget->removeWidget(detailPage);
                detailPage->deleteLater();
            });
            m_stackWidget->addWidget(detailPage);
            m_stackWidget->setCurrentWidget(detailPage);
        });

        QPushButton *addToFavorButton=new QPushButton("添加到收藏夹",this);
        connect(addToFavorButton,&QPushButton::clicked,[this,c](){
            if(!m_user->getFavorites().contains(c)){

                m_user->getFavorites().push_back(c);
                QMessageBox::information(this,"Tips","Success!");
                emit favoritesUpdated();
                
            }
            else{
                QMessageBox::information(this,"Tips","请勿重复添加课程！");
            }
        });

        QPushButton *electCourseButton=new QPushButton("选课",this);
        connect(electCourseButton,&QPushButton::clicked,[this,c]{
            bool ok;
            int votees=QInputDialog::getInt(this,"海淀赌场","投点数：",0,0,100,1,&ok);
            if(!ok){
                return;
            }
            
            
            QSet<QPair<QString,QString>> existingCourses;//(time,week);
            for(const auto& course: m_user->getCurrentCourses()){
                for(const auto& time: course.timeList){
                    for(const auto&w: course.week){
                        existingCourses.insert(qMakePair(time,w));
                    }
                }
            }

            bool hasConflict=false;
            if(votees>m_user->getRemainingPoints()){
                QMessageBox::information(this,"Tips","超额投点.");
                return;
            }
            for(const auto&time:c.timeList){
                for(const auto& w:c.week){
                    if(existingCourses.contains(qMakePair(time,w))){
                        hasConflict=true;
                        break;
                    }
                }
            }

            if(hasConflict){
                QMessageBox::information(this,"Tips","选课失败，与已选课程时间冲突！");
            }
            else{
                m_user->getCurrentCourses().push_back(c);
                m_user->setPointForCourse(c.code,votees);
                QMessageBox::information(this,"Tips","选课成功！");
                emit coursesUpdated();
            }
        });
        // 在populateCourseTable函数中
        // 创建一个容器来放置3个按钮
        QWidget *buttonContainer = new QWidget();
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(2, 2, 2, 2);
        buttonLayout->setSpacing(5);
        
        // 添加3个按钮到布局
        buttonLayout->addWidget(detailButton);
        buttonLayout->addWidget(addToFavorButton);
        buttonLayout->addWidget(electCourseButton);
        
        // 将容器设置到表格单元格
        m_courseTable->setCellWidget(i, 5, buttonContainer);
        
        
        // 可以在这里连接按钮的信号
    }
}

void CourseSearchPage::onSearchButtonClicked() {
    QString keyword = m_searchLineEdit->text().trimmed();
    if (keyword.isEmpty()) {
        populateCourseTable(m_allCourses);
        return;
    }

    QVector<CourseInfo> filteredCourses;
    for (const auto &course : m_allCourses) {
        if (course.name.contains(keyword) ||
            course.code.contains(keyword) ||
            course.teacherList.join("").contains(keyword) ||
            course.timeList.join("").contains(keyword)) {
            filteredCourses.append(course);
        }
    }

    populateCourseTable(filteredCourses);
}




