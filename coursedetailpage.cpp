// CourseDetailPage.cpp
#include "coursedetailpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>

CourseDetailPage::CourseDetailPage(const CourseInfo& course, QWidget *parent)
    : QWidget(parent) {
    setupUI(course);
}

// CourseDetailPage.cpp
void CourseDetailPage::setupUI(const CourseInfo& course) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 课程标题区
    QLabel *titleLabel = new QLabel(course.name, this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    // 基本信息区
    QWidget *infoSection = createInfoSection(course);

    // 课程介绍区
    QWidget *introSection = createIntroSection(course);

    // 操作按钮区
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *backButton = new QPushButton("返回列表", this);
    QPushButton *enrollButton = new QPushButton("选课", this);
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(enrollButton);

    // 组装界面
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoSection);
    mainLayout->addWidget(introSection);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(backButton, &QPushButton::clicked, this, &CourseDetailPage::backRequested);
    connect(enrollButton, &QPushButton::clicked, [this, course]() {
        emit enrollRequested(course.code);
    });
}

QLabel* CourseDetailPage::createSectionTitle(const QString& text) {
    QLabel *label = new QLabel(text, this);
    label->setStyleSheet(R"(
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #3498db;
            border-bottom: 1px solid #e0e0e0;
            padding-bottom: 5px;
            margin-top: 15px;
        }
    )");
    return label;
}

QWidget* CourseDetailPage::createInfoSection(const CourseInfo& course) {
    QWidget *container = new QWidget(this);
    QFormLayout *formLayout = new QFormLayout(container);
    formLayout->setHorizontalSpacing(20);
    formLayout->setVerticalSpacing(10);

    // 基本信息标题
    formLayout->addRow(createSectionTitle("课程基本信息"));

    // 基本信息行
    formLayout->addRow("课程编号:", new QLabel(course.code, this));
    formLayout->addRow("开课单位:", new QLabel(course.unit, this));
    formLayout->addRow("课程类型:", new QLabel(course.type, this));
    formLayout->addRow("学分:", new QLabel(course.score, this));
    formLayout->addRow("上课周数:", new QLabel(course.week, this));
    formLayout->addRow("上课时间:", new QLabel(course.timeList.join("\n"), this));
    formLayout->addRow("授课教师:", new QLabel(course.teacherList.join(", "), this));
    formLayout->addRow("先修课程:", new QLabel(course.firstClass, this));
    formLayout->addRow("备注信息:", new QLabel(course.info, this));

    // 设置标签样式
    for (int i = 0; i < formLayout->rowCount(); ++i) {
        QLayoutItem *labelItem = formLayout->itemAt(i, QFormLayout::LabelRole);
        if (labelItem && labelItem->widget()) {
            labelItem->widget()->setStyleSheet("font-weight: bold; color: #7f8c8d;");
        }
    }

    return container;
}

QWidget* CourseDetailPage::createIntroSection(const CourseInfo& course) {
    QWidget *container = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(container);

    // 中文介绍
    layout->addWidget(createSectionTitle("中文介绍"));
    QTextEdit *chineseIntro = new QTextEdit(course.chineseIntro, this);
    chineseIntro->setReadOnly(true);
    chineseIntro->setFrameShape(QFrame::NoFrame);
    chineseIntro->setStyleSheet("background: transparent;");
    layout->addWidget(chineseIntro);

    // 英文介绍
    layout->addWidget(createSectionTitle("English Introduction"));
    QTextEdit *englishIntro = new QTextEdit(course.englishIntro, this);
    englishIntro->setReadOnly(true);
    englishIntro->setFrameShape(QFrame::NoFrame);
    englishIntro->setStyleSheet("background: transparent;");
    layout->addWidget(englishIntro);

    return container;
}
