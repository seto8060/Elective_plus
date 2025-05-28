// CourseSelectionPage.cpp
#include "CourseSelectPage.h"
#include "coursedetailpage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStackedWidget>  // 确保包含这个头文件

CourseSelectionPage::CourseSelectionPage(const QVector<CourseInfo>& allCourses, QWidget *parent)
    : QWidget(parent), m_allCourses(allCourses) {
    setupUI();
    populateCourseTable(allCourses);
}

void CourseSelectionPage::setupUI() {
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

    // 操作按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addToFavoritesButton = new QPushButton("添加到收藏夹", this);
    m_enrollButton = new QPushButton("选课", this);
    buttonLayout->addWidget(m_addToFavoritesButton);
    buttonLayout->addWidget(m_enrollButton);

    // 添加到内容布局
    contentLayout->addLayout(searchLayout);
    contentLayout->addWidget(m_courseTable);
    contentLayout->addLayout(buttonLayout);
    
    // 将主内容窗口添加到堆叠窗口
    m_stackWidget->addWidget(mainContentWidget);
    
    // 将堆叠窗口添加到主布局
    mainLayout->addWidget(m_stackWidget);

    // 连接信号槽
    connect(m_searchButton, &QPushButton::clicked, this, &CourseSelectionPage::onSearchButtonClicked);
    connect(m_addToFavoritesButton, &QPushButton::clicked, this, &CourseSelectionPage::onAddToFavoritesClicked);
    connect(m_enrollButton, &QPushButton::clicked, this, &CourseSelectionPage::onEnrollCourseClicked);
}

void CourseSelectionPage::populateCourseTable(const QVector<CourseInfo>& courses) {
    m_courseTable->setRowCount(courses.size());

    for (int i = 0; i < courses.size(); ++i) {
        const CourseInfo &c = courses[i];
        m_courseTable->setItem(i, 0, new QTableWidgetItem(c.code));
        m_courseTable->setItem(i, 1, new QTableWidgetItem(c.name));
        m_courseTable->setItem(i, 2, new QTableWidgetItem(c.teacherList.join(", ")));
        m_courseTable->setItem(i, 3, new QTableWidgetItem(c.timeList.join("；")));
        m_courseTable->setItem(i, 4, new QTableWidgetItem(c.unit));

        // 添加操作按钮
        QPushButton *detailButton = new QPushButton("查看详情", this);
        connect(detailButton, &QPushButton::clicked, [this, c]() {
            CourseDetailPage *detailPage = new CourseDetailPage(c, this);
            connect(detailPage, &CourseDetailPage::backRequested, [this, detailPage]() {
                m_stackWidget->removeWidget(detailPage);
                detailPage->deleteLater();
            });
            m_stackWidget->addWidget(detailPage);
            m_stackWidget->setCurrentWidget(detailPage);
        });
        m_courseTable->setCellWidget(i, 5, detailButton);
        // 可以在这里连接按钮的信号
    }
}

void CourseSelectionPage::onSearchButtonClicked() {
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

void CourseSelectionPage::onAddToFavoritesClicked() {
    // 实现添加到收藏夹逻辑
}

void CourseSelectionPage::onEnrollCourseClicked() {
    // 实现选课逻辑
}
