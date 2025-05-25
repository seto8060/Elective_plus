#include "HomePage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>

HomePage::HomePage(const UserInfo &user,QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 欢迎语
    QLabel *welcomeLabel = new QLabel("欢迎使用选课网++", this);
    welcomeLabel->setStyleSheet("font-size: 26px; font-weight: bold; color: #2d3436;");
    welcomeLabel->setAlignment(Qt::AlignCenter);

    QLabel *tipLabel = new QLabel("请从左侧选择模块开始使用", this);
    tipLabel->setStyleSheet("font-size: 16px; color: #636e72;");
    tipLabel->setAlignment(Qt::AlignCenter);
    QString info = QString("当前登录用户：%1（%2 %3）")
                       .arg(user.getUsername())
                       .arg(user.getGrade())
                       .arg(user.getCollege());

    QLabel *userLabel = new QLabel(info, this);
    // userLabel->setAlignment(Qt::AlignCenter);

    QLabel *line = new QLabel(this);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #dfe6e9;");

    QString introText = R"(
本系统旨在为用户提供一个集选课、测评、智能推荐于一体的综合平台。<br><br>

<b>【选课模块】</b><br>
支持课程查询（按名称、时间、教师、单位等）、学生选课与投点、教务后台管理。历史选课记录将辅助测评与推荐系统。<br><br>

<b>【课程测评模块】</b><br>
可为选过的课程打分和写评语（如给分、任务量、听感）。测评分数将影响课程展示排序。<br><br>

<b>【智能选课系统】</b><br>
根据课程时间冲突、用户偏好（如不上早八、任务轻松）和投点策略，智能生成推荐课表方案，供用户参考。<br><br>

<b>【附属功能】</b><br>
收藏夹支持一键收藏，历史选课页面支持测评回顾。系统整体支持清晰的 UI 展示与高效的课程管理。<br>
)";

    QLabel *introLabel = new QLabel(this);
    introLabel->setText(introText);
    introLabel->setStyleSheet("font-size: 14px; line-height: 1.6em;");
    introLabel->setWordWrap(true);
    introLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(introLabel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    mainLayout->addSpacing(20);
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(line);
    mainLayout->addWidget(scrollArea);
    // mainLayout->setStretch(3, 1);

    setLayout(mainLayout);
}
