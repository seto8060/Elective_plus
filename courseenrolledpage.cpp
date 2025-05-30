#include "courseenrolledpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

courseEnrolledPage::courseEnrolledPage(UserInfo* user, QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    populateCourseTable(user->getCurrentCourses());
}

void courseEnrolledPage::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QLabel *titleLabel = new QLabel("已选课程列表", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // 课程表格
    coursesTable = new QTableWidget(this);
    coursesTable->setColumnCount(10);
    coursesTable->setHorizontalHeaderLabels({
        "课程代码", "课程名称", "学分", "类型",
        "班级", "上课周数", "上课时间", "授课教师",
        "人数", "操作"
    });
    coursesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    coursesTable->horizontalHeader()->setStretchLastSection(true);
    coursesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coursesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coursesTable->setAlternatingRowColors(true);
    mainLayout->addWidget(coursesTable);

    // 底部按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *backButton = new QPushButton("返回", this);
    backButton->setStyleSheet("padding: 8px 16px;");
    buttonLayout->addWidget(backButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(backButton, &QPushButton::clicked, this, &courseEnrolledPage::backRequested);
    
}

void courseEnrolledPage::populateCourseTable(QVector<CourseInfo>& courses)
{
    // 保存当前列宽
    QList<int> columnWidths;
    for (int i = 0; i < coursesTable->columnCount(); ++i) {
        columnWidths.append(coursesTable->columnWidth(i));
    }

    // 原有表格更新逻辑
    coursesTable->setRowCount(0);

    if (courses.isEmpty()) {
        coursesTable->setRowCount(1);
        QLabel *emptyLabel = new QLabel("当前没有已选课程", coursesTable);
        coursesTable->setCellWidget(0, 0, emptyLabel);
        coursesTable->setSpan(0, 0, 1, coursesTable->columnCount());
        return;
    }

    for (const CourseInfo &course : courses) {
        int row = coursesTable->rowCount();
        coursesTable->insertRow(row);

        // 填充课程信息
        coursesTable->setItem(row, 0, new QTableWidgetItem(course.code));
        coursesTable->setItem(row, 1, new QTableWidgetItem(course.name));
        coursesTable->setItem(row, 2, new QTableWidgetItem(course.unit));
        coursesTable->setItem(row, 3, new QTableWidgetItem(course.type));
        coursesTable->setItem(row, 4, new QTableWidgetItem(course.classNumber));
        coursesTable->setItem(row, 5, new QTableWidgetItem(course.week));

        // 处理时间和教师列表
        QString timeStr = course.timeList.join("\n");
        QString teacherStr = course.teacherList.join("\n");

        coursesTable->setItem(row, 6, new QTableWidgetItem(timeStr));
        coursesTable->setItem(row, 7, new QTableWidgetItem(teacherStr));

        // 显示人数信息
        QString personInfo = QString("%1/%2").arg(course.Now_person).arg(course.Max_person);
        coursesTable->setItem(row, 8, new QTableWidgetItem(personInfo));

        // 添加退课按钮
        //退课按钮的功能
        QPushButton *dropButton = new QPushButton("退课", coursesTable);
        dropButton->setProperty("courseCode", course.code);
        dropButton->setStyleSheet("padding: 2px 5px;");

        connect(dropButton, &QPushButton::clicked, [this, course]() {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "确认退课",
                                          QString("确定要退选课程: %1 (%2)?").arg(course.name).arg(course.code),
                                          QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                emit dropCourseRequested(course.code);
            }
        });

        coursesTable->setCellWidget(row, 9, dropButton);
        //退课按钮的实现
        // 设置行高以适应多行文本
        coursesTable->setRowHeight(row, qMax(30, (timeStr.count('\n') + 1) * 20));
    }

    // 恢复列宽
    for (int i = 0; i < columnWidths.size() && i < coursesTable->columnCount(); ++i) {
        coursesTable->setColumnWidth(i, columnWidths[i]);
    }

    // 确保表格填满可用空间
    coursesTable->horizontalHeader()->setStretchLastSection(true);
}


