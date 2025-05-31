#include "HomePage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QPainter>

HomePage::HomePage(UserInfo *user,QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // QPixmap bg(":/resources/resources/background.png");
    // bg = bg.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // QPalette palette;
    // palette.setBrush(QPalette::Window, QBrush(bg));
    this->setAutoFillBackground(false);
    // this->setPalette(palette);

    // 欢迎语
    QLabel *welcomeLabel = new QLabel("欢迎使用选课网++！", this);
    welcomeLabel->setStyleSheet("font-size: 35px; font-weight: bold; color: #2d3436;");
    welcomeLabel->setAlignment(Qt::AlignCenter);

    // QLabel *tipLabel = new QLabel("请从左侧选择模块开始使用", this);
    // tipLabel->setStyleSheet("font-size: 16px; color: #636e72;");
    // tipLabel->setAlignment(Qt::AlignCenter);
    QString info = QString(
                       "当前登录用户：%1（%2 %3）<br>"
                       "真实姓名：%4<br>"
                       "学号：%5")
                       .arg(user->getUsername())
                       .arg(user->getGrade())
                       .arg(user->getCollege())
                       .arg(user->getRealname())
                       .arg(user->getIndex());

    QLabel *userLabel = new QLabel(info, this);
    userLabel->setTextFormat(Qt::RichText);
    userLabel->setStyleSheet("font-size: 16px; color: #2d3436; font-weight: 500;");
    // userLabel->setAlignment(Qt::AlignCenter);

    QLabel *line = new QLabel(this);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #dfe6e9;");

    QString introText = R"(
<div style="font-family:'微软雅黑'; font-size:17px; color:#2d3436; line-height:2em;">

<p>这是一个集 <b style="color:#2d3436;">选课</b>、<b style="color:#2d3436;">测评</b>、<b style="color:#2d3436;">收藏</b>、<b style="color:#2d3436;">智能推荐</b> 于一体的课程平台。您可以在左侧选择对应模块进入使用。</p>

<hr style="border:none; border-top:1px solid #ccc; margin:20px 0;">

<!-- 模块说明块开始 -->
<div style="margin-bottom:30px;">
    <p style="margin:0; font-size:18px; font-weight:bold;">📋 选课</p>
    <p style="margin:5px 0 0 28px;">浏览课程信息（课程号、名称、时间、教师、开课单位等），进行选课投点操作，查看当前选课情况。</p>
</div>

<div style="margin-bottom:30px;">
    <p style="margin:0; font-size:18px; font-weight:bold;">🤖 智能选课系统</p>
    <p style="margin:5px 0 0 28px;">根据时间冲突检测、用户偏好（如不上早八、任务轻松）、投点策略，智能生成推荐课表，助您在“海淀大赌场”里满载而归！</p>
</div>

<div style="margin-bottom:30px;">
    <p style="margin:0; font-size:18px; font-weight:bold;">🧡 收藏夹</p>
    <p style="margin:5px 0 0 28px;">支持将感兴趣课程一键收藏，便于快速查看与管理。</p>
</div>

<div style="margin-bottom:30px;">
    <p style="margin:0; font-size:18px; font-weight:bold;">🗂️ 选课列表</p>
    <p style="margin:5px 0 0 28px;">展示您当前学期所选课程和状态（通过课程表、选课列表两种方式呈现），方便整体课表管理。</p>
</div>

<div style="margin-bottom:30px;">
    <p style="margin:0; font-size:18px; font-weight:bold;">🕒 历史选课</p>
    <p style="margin:5px 0 0 28px;">支持浏览历年选课记录，并对往年课程进行量化测评，为未来选课提供参考。</p>
</div>

<p style="font-size:25px;font-weight: bold;margin-top:30px;">祝您选课愉快！😊</p>

</div>
)";

    QLabel *introLabel = new QLabel(this);
    introLabel->setTextFormat(Qt::RichText);
    introLabel->setText(introText);
    introLabel->setStyleSheet("font-size: 14px; line-height: 1.6em;");
    introLabel->setWordWrap(true);
    introLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(introLabel);
    scrollArea->setWidgetResizable(true);
    introLabel->setAttribute(Qt::WA_TranslucentBackground);
    scrollArea->setStyleSheet("background: transparent;");
    scrollArea->setFrameShape(QFrame::NoFrame);

    mainLayout->addSpacing(20);
    mainLayout->addWidget(welcomeLabel);
    // mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(line);
    mainLayout->addWidget(scrollArea);
    QPixmap logoPixmap(":/resources/pictures/logo.png");
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(logoPixmap.scaled(260, 260, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(logoLabel);
    mainLayout->addLayout(logoLayout);
    // mainLayout->setStretch(3, 1);

    setLayout(mainLayout);
}
void HomePage::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPixmap bg(":/resources/resources/background.png");
    if (!bg.isNull()) {
        painter.drawPixmap(this->rect(), bg);
    } else {
        painter.fillRect(rect(), Qt::lightGray);
    }
}
