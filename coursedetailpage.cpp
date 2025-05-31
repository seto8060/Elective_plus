// CourseDetailPage.cpp
#include "coursedetailpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>

CourseDetailPage::CourseDetailPage(const CourseInfo& course,courseComment* comment,QWidget *parent)
    : QWidget(parent) {
    setupUI(course,comment);
}

// CourseDetailPage.cpp
void CourseDetailPage::setupUI(const CourseInfo& course, courseComment* comment) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ================= 上部左右分栏 =================
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 左侧区域（标题 + 基本信息）
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0,0,0,0);
    titleLayout->setSpacing(10);

    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel(course.name, this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    titleLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    leftLayout->addWidget(titleLabel);

    QPushButton *enrollButton = new QPushButton("选课", this);
    enrollButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    enrollButton->setStyleSheet(R"(
    QPushButton {
        padding: 6px 12px;
        background-color: #2ecc71;
        color: white;
        border-radius: 4px;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #27ae60;
    }
)");

    titleLayout->addWidget(titleLabel,0,Qt::AlignLeft);
    titleLayout->addWidget(enrollButton,0,Qt::AlignRight);
    QWidget *titleContainer = new QWidget(this);
    titleContainer->setLayout(titleLayout);

    leftLayout->addWidget(titleContainer);

    QWidget *infoSection = createInfoSection(course);
    leftLayout->addWidget(infoSection);
    leftLayout->addStretch();


    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setSpacing(15);

    // 评论区域
    QWidget *commentsSection = createCommentsSection(comment);


    rightLayout->addWidget(commentsSection);
    rightLayout->addStretch();

    // 组装上部
    topLayout->addWidget(leftWidget, 1);  // 左侧占 2/3 宽度
    topLayout->addWidget(rightWidget, 1); // 右侧占 1/3 宽度
    topLayout->setContentsMargins(0, 0, 0, 0);

    // ================= 下部区域 =================

    QWidget *bottomWidget = new QWidget();
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);
    bottomLayout->addWidget(createSectionTitle("课程介绍"));
    QWidget *introSection = createIntroSection(course);
    bottomLayout->addWidget(introSection);


    // 操作按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *backButton = new QPushButton("返回列表", this);

    buttonLayout->addWidget(backButton,Qt::AlignCenter);
    buttonLayout->addStretch();
    bottomLayout->addLayout(buttonLayout);

    // ================= 组装整个界面 =================
    mainLayout->addLayout(topLayout);         // 上部
    mainLayout->addWidget(bottomWidget, 2);   // 下部（1 为伸缩因子）

    // ================= 信号连接 =================
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
    QVBoxLayout *layout = new QVBoxLayout(container);

    // 标题
    layout->addWidget(createSectionTitle("课程基本信息"));

    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setHorizontalSpacing(20);
    formLayout->setVerticalSpacing(10);

    // 填充基本信息
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

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true); // 自动调整内容大小
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // 自动显示垂直滚动条

    // 创建内容容器并设置表单布局
    QWidget *formContainer = new QWidget();
    formContainer->setLayout(formLayout);
    scrollArea->setWidget(formContainer);

    // 设置滚动区域的最小高度（防止窗口过小）
    scrollArea->setMinimumHeight(200);

    // 添加到主布局
    layout->addWidget(scrollArea);

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

QWidget* CourseDetailPage::createCommentsSection(courseComment* comment) {
    QWidget *container = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(container);

    // 评论标题
    layout->addWidget(createSectionTitle("课程评价"));

    m_comments=comment->comments;
    // 评论展示区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);

    QWidget *commentsContainer = new QWidget();
    m_commentsLayout=new QVBoxLayout(commentsContainer);
    m_commentsLayout->setSpacing(15);
    m_commentsLayout->setContentsMargins(5,5,5,5);

    scrollArea->setWidget(commentsContainer);
    layout->addWidget(scrollArea);
    // 翻页按钮
    QHBoxLayout *pageLayout = new QHBoxLayout();
    QPushButton *prevButton = new QPushButton("上一页", this);
    QPushButton *nextButton = new QPushButton("下一页", this);
    pageLayout->addWidget(prevButton);
    pageLayout->addStretch();
    pageLayout->addWidget(nextButton);
    layout->addLayout(pageLayout);

    // 连接信号槽
    connect(prevButton, &QPushButton::clicked, [this]() {
        if (m_currentPage > 0) {
            m_currentPage--;
            updateCommentsDisplay();
        }
    });

    connect(nextButton, &QPushButton::clicked, [this]() {
        if ((m_currentPage + 1) * COMMENTS_PER_PAGE < m_comments.size()) {
            m_currentPage++;
            updateCommentsDisplay();
        }
    });

    // 初始显示
    updateCommentsDisplay();

    return container;
}

void CourseDetailPage::updateCommentsDisplay() {
    // 清空现有评论
    QLayoutItem *child;
    while ((child = m_commentsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    if(m_comments.isEmpty()){
        QLabel* noCommentLabel = new QLabel("暂无课程评价",this);
        noCommentLabel->setAlignment(Qt::AlignCenter);
        noCommentLabel->setStyleSheet("color:#95a5a6; font-style: italic;");
        m_commentsLayout->addWidget(noCommentLabel);
    }
    // 显示当前页评论
    int start = m_currentPage * COMMENTS_PER_PAGE;
    int end = qMin(start + COMMENTS_PER_PAGE, m_comments.size());

    for (int i = start; i < end; i++) {
        auto comment = m_comments[i];
        QWidget *commentWidget = new QWidget();
        commentWidget->setStyleSheet(R"(
            QWidget {
                background: #f9f9f9;
                border-radius: 5px;
                padding: 10px;
                border: 1px solid #e0e0e0;
            }
        )");
        QVBoxLayout *commentLayout = new QVBoxLayout(commentWidget);
        commentLayout->setSpacing(5);
        QLabel *criticLabel = new QLabel("评价人: " + comment.critic);
        QLabel *lisPreLabel=new QLabel("听感："+QString::number(comment.listenPrefer));
        QLabel *hwPrefer=new QLabel("作业量："+QString::number(comment.hwPrefer));
        QLabel *scorePrefer=new QLabel("给分："+QString::number(comment.scorePrefer));

        QLabel *contentLabel = new QLabel("留言："+comment.content);
        contentLabel->setWordWrap(true);
        QLabel *priorityLabel = new QLabel("评分: " + QString::number(comment.priority));

        commentLayout->addWidget(criticLabel);
        commentLayout->addWidget(lisPreLabel);
        commentLayout->addWidget(hwPrefer);
        commentLayout->addWidget(scorePrefer);
        commentLayout->addWidget(contentLabel);
        commentLayout->addWidget(priorityLabel);

        m_commentsLayout->addWidget(commentWidget);
    }
    m_commentsLayout->addStretch();
}
