#include "teacherwindow.h"
#include "term.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QSpacerItem>
#include <QToolButton>

TeacherWindow::TeacherWindow(QWidget *parent)
    : QWidget(parent), teacherInfo(new TeacherInfo(this))
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createMainPage());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}

QWidget* TeacherWindow::createMainPage() {
    QWidget *page = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(page);

    QLabel *infoLabel = new QLabel(page);
    infoLabel->setText(
        QString("当前学期：%1\n当前选课学期：%2")
            .arg(teacherInfo->getCurrentTerm().toString())
            .arg(teacherInfo->getEnrollmentTerm().isValid() ? teacherInfo->getEnrollmentTerm().toString() : "无")
        );
    infoLabel->setAlignment(Qt::AlignLeft);
    infoLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(infoLabel);

    QGridLayout *grid = new QGridLayout;

    struct ButtonInfo {
        QString text;
        QString iconPath;
    };

    QList<ButtonInfo> buttons = {
        { "编辑课程列表", ":/resources/icon/edit.svg" },
        { "手工选课", ":/resources/icon/manual.svg" },
        { "修改当前学期", ":/resources/icon/semester.svg" },
        { "抽签", ":/resources/icon/lottery.svg" },
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
        grid->addWidget(button, row, col);
        button->setProperty("actionName", info.text);

        Term operateTerm(-1,-1);
        if (info.text == "编辑课程列表")
            operateTerm = teacherInfo->canModifyCourse();
        else if (info.text == "手工选课")
            operateTerm = teacherInfo->canManuallyElect();
        else if (info.text == "抽签")
            operateTerm = teacherInfo->canDoLottery();
        else if (info.text == "导出信息")
            operateTerm = teacherInfo->canExportStudentInfo();
        else if (info.text == "批量操作用户")
            operateTerm = teacherInfo->canBatchUpdateUsers() ? teacherInfo->getCurrentTerm() : Term(-1, -1);
        else if (info.text == "修改当前学期")
            operateTerm = teacherInfo->getCurrentTerm();

        bool available = operateTerm.isValid();
        button->setEnabled(available);
        if (!available) button->setToolTip("当前不可用");

        connect(button, &QToolButton::clicked, this, [=]() {
            showSubPage(info.text, operateTerm);
        });

        col++;
        if (col >= 3) { col = 0; row++; }
    }

    mainLayout->addLayout(grid);
    page->setLayout(mainLayout);
    return page;
}

QWidget* TeacherWindow::createSubPage(const QString &pageName, const Term &operateTerm) {
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    QPushButton *backButton = new QPushButton("← 返回", page);
    connect(backButton, &QPushButton::clicked, this, &TeacherWindow::showMainPage);

    QLabel *label = new QLabel(
        QString("功能：%1\n操作学期：%2（待实现）")
            .arg(pageName)
            .arg(operateTerm.isValid() ? operateTerm.toString() : "无效"), page
        );
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    page->setLayout(layout);
    return page;
}

void TeacherWindow::showSubPage(const QString &pageName, const Term &operateTerm) {
    QString key = pageName + operateTerm.toString();

    if (!subPages.contains(key)) {
        QWidget *subPage = createSubPage(pageName, operateTerm);
        subPages[key] = subPage;
        stackedWidget->addWidget(subPage);
    }
    stackedWidget->setCurrentWidget(subPages[key]);
}

void TeacherWindow::showMainPage() {
    stackedWidget->setCurrentIndex(0);
}
