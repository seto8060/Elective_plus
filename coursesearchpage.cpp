#include "coursesearchpage.h"
#include "coursedetailpage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStackedWidget>  // 确保包含这个头文件
#include <QDebug>
#include<QInputDialog>

CourseSearchPage::CourseSearchPage(const QVector<CourseInfo>& allCourses, UserInfo* user,QVector<courseComment>* courseComments,QWidget *parent)
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
    m_courseTable->setColumnCount(7); // 编号、名称、教师、时间、单位、操作、marks
    m_courseTable->setHorizontalHeaderLabels({"课程编号", "课程名称", "授课教师", "上课时间", "开课单位", "操作","评分"});
    m_courseTable->horizontalHeader()->setStretchLastSection(false);
    m_courseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_courseTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_courseTable->setColumnWidth(0, 130);   // 课程编号
    m_courseTable->setColumnWidth(1, 150);  // 课程名称
    m_courseTable->setColumnWidth(2, 100);  // 授课教师
    m_courseTable->setColumnWidth(3, 350);  // 上课时间
    m_courseTable->setColumnWidth(4, 150);  // 开课单位
    m_courseTable->setColumnWidth(5, 300);  // 操作按钮
    m_courseTable->setColumnWidth(6, 80);   // 评分

    // 启用换行
    m_courseTable->setWordWrap(true);
    m_courseTable->resizeRowsToContents();
    m_courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

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


        
        // 修改后:
        courseComment* tar = nullptr;
        for(auto& v:*m_courseComments){
            if(v.code==c.code){
                tar=&v;
                break;
            }
        }
        if(!tar){
            tar = &m_courseComments->emplace_back(courseComment{c.code});
        }
        QVector<comment> thisCourseComments;
        for(const auto& v:tar->comments){
            if(v.teacher.join("；")==c.teacherList.join("")){
                
                // qDebug()<<"+1";
                thisCourseComments.append(v);
            }
        }
        int cnt=thisCourseComments.size();
        double total,listen,exam,hw;
        total=listen=exam=hw=0;
        for(const auto& v:thisCourseComments){
            total+=v.priority;
            listen+=v.listenPrefer;
            exam+=v.scorePrefer;
            hw+=v.hwPrefer;
        }

        total=total/cnt;
        listen=listen/cnt;
        exam=exam/cnt;
        hw=hw/cnt;

        QTableWidgetItem *ratingItem = new QTableWidgetItem();
        setRatingDisplay(ratingItem, total,hw,exam,listen,cnt);
        m_courseTable->setItem(i, 6, ratingItem);
        // 添加操作按钮
        QPushButton *detailButton = new QPushButton("查看详情", this);
        connect(detailButton, &QPushButton::clicked, [this, tar,c]() {
            CourseDetailPage *detailPage = new CourseDetailPage(c, tar,this);
            connect(detailPage, &CourseDetailPage::backRequested, [this, detailPage]() {
                m_stackWidget->removeWidget(detailPage);
                detailPage->deleteLater();
            });
            connect(detailPage, &CourseDetailPage::enrollRequested, this, [this, c](const QString& code) {
                if (code == c.code) {
                    handleElectCourse(c);
                } else {
                    QMessageBox::warning(this, "错误", "课程编号不匹配，请刷新页面后重试。");
                }
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
            handleElectCourse(c);
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

void CourseSearchPage::handleElectCourse(const CourseInfo& course) {
    if (m_isHandlingEnroll) return;
    m_isHandlingEnroll = true;
    bool ok;
    int votees = QInputDialog::getInt(this, "海淀赌场", "投点数：", 0, 0, 100, 1, &ok);
    if (!ok) return;

    QSet<QPair<QString, QString>> existingCourses;
    for (const auto& enrolled : m_user->getCurrentCourses()) {
        for (const auto& time : enrolled.timeList) {
            for (const auto& w : enrolled.week) {
                existingCourses.insert({time, w});
            }
        }
    }

    bool hasConflict = false;
    if (votees > m_user->getRemainingPoints()) {
        QMessageBox::information(this, "Tips", "超额投点。");
        return;
    }

    for (const auto& time : course.timeList) {
        for (const auto& w : course.week) {
            if (existingCourses.contains({time, w})) {
                hasConflict = true;
                break;
            }
        }
        if (hasConflict) break;
    }

    if (hasConflict) {
        QMessageBox::information(this, "Tips", "选课失败，与已选课程时间冲突！");
    } else {
        m_user->getCurrentCourses().push_back(course);
        m_user->setPointForCourse(course.code, votees);
        QMessageBox::information(this, "Tips", "选课成功！");
        emit coursesUpdated();
    }
    m_isHandlingEnroll=false;
}

void CourseSearchPage::setRatingDisplay(QTableWidgetItem *item, double total,double hw,double exam, double listen,int cnt)
{
    QString emoji;
    if(cnt==0){
        emoji="😶‍🌫️";
    }
    else{
        if (total >= 4) {
            emoji = "😍";
        } else if (total >= 2.0) {
            emoji = "😑";
        } else {
            emoji = "😞";
        }

    }

    item->setText(emoji);
    item->setTextAlignment(Qt::AlignCenter);

    // 设置 ToolTip 提示
    QString toolTip = QString("评价数：%1\n总评: %2\n听感: %3\n作业量: %4\n给分: %5")
                          .arg(cnt)
                          .arg(total)
                          .arg(listen)
                          .arg(hw)
                          .arg(exam);
    item->setToolTip(toolTip);
}


