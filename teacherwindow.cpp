#include "teacherwindow.h"
#include "userinfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QSpacerItem>
#include <QToolButton>

TeacherWindow::TeacherWindow(UserInfo *userinfo, QWidget *parent) : QWidget(parent) {
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createMainPage());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}

QWidget* TeacherWindow::createMainPage() {
    QWidget *page = new QWidget(this);
    QGridLayout *layout = new QGridLayout(page);

    struct ButtonInfo {
        QString text;
        QString iconPath;
    };

    QList<ButtonInfo> buttons = {
        { "加/减/改课", ":/resources/icon/edit.svg" },
        { "手工选课", ":/resources/icon/manual.svg" },
        { "开始/结束学期", ":/resources/icon/semester.svg" },
        { "抽签！", ":/resources/icon/lottery.svg" },
        { "导出信息", ":/resources/icon/export.svg" },
        { "批量操作用户", ":/resources/icon/batch.svg" }
    };

    int row = 0, col = 0;
    for (const auto &info : buttons) {
        QToolButton *button = new QToolButton(page);
        button->setText(info.text);
        button->setIcon(QIcon(info.iconPath));
        button->setIconSize(QSize(40, 40));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setMinimumSize(100, 100);
        layout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, [=]() {
            showSubPage(info.text);
        });

        col++;
        if (col >= 3) { col = 0; row++; }
    }

    layout->setAlignment(Qt::AlignTop);
    page->setLayout(layout);
    return page;
}

QWidget* TeacherWindow::createSubPage(const QString &pageName) {
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    QPushButton *backButton = new QPushButton("← 返回", page);
    connect(backButton, &QPushButton::clicked, this, &TeacherWindow::showMainPage);

    QLabel *label = new QLabel("功能：" + pageName + "（待实现）", page);
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    page->setLayout(layout);
    return page;
}

void TeacherWindow::showSubPage(const QString &pageName) {
    if (!subPages.contains(pageName)) {
        QWidget *subPage = createSubPage(pageName);
        subPages[pageName] = subPage;
        stackedWidget->addWidget(subPage);
    }
    stackedWidget->setCurrentWidget(subPages[pageName]);
}

void TeacherWindow::showMainPage() {
    stackedWidget->setCurrentIndex(0);
}
