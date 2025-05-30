#include "teacherwindow.h"
#include "term.h"
#include "CourseLoader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QSpacerItem>
#include <QToolButton>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include "courseinfo.h"
#include <QJsonArray>
bool TeacherWindow::checkConflict(const QString &userId, const QString &courseId) {
    // TODO: 实际检查课表冲突
    return false;
}
TeacherWindow::TeacherWindow(QWidget *parent)
    : QWidget(parent), teacherInfo(new TeacherInfo(this))
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createMainPage());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}
void TeacherWindow::refreshMainPage() {
    if (!infoLabel) return;

    infoLabel->setText(
        QString("当前学期：%1\n当前选课学期：%2")
            .arg(teacherInfo->getCurrentTerm().toString())
            .arg(teacherInfo->getEnrollmentTerm().isValid() ? teacherInfo->getEnrollmentTerm().toString() : "无")
        );

    for (int i = 0; i < functionButtons.size(); ++i) {
        QToolButton *btn = functionButtons[i];
        QString text = btn->property("actionName").toString();
        Term operateTerm(-1, -1);

        if (text == "编辑课程列表")
            operateTerm = teacherInfo->canModifyCourse();
        else if (text == "手工选课")
            operateTerm = teacherInfo->canManuallyElect();
        else if (text == "抽签")
            operateTerm = teacherInfo->canDoLottery();
        else if (text == "导出信息")
            operateTerm = teacherInfo->canExportStudentInfo();
        else if (text == "刷新验证码") ;
        else if (text == "修改当前学期")
            operateTerm = teacherInfo->getCurrentTerm();

        functionTerms[i] = operateTerm;
        bool available = operateTerm.isValid();
        if (text == "刷新验证码") available = true;
        btn->setEnabled(available);
        btn->setToolTip(available ? "" : "当前不可用");
    }
}
QWidget* TeacherWindow::createMainPage() {
    QWidget *page = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(page);

    infoLabel = new QLabel(page);
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
        { "刷新验证码", ":/resources/icon/reset.svg" }
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
        else if (info.text == "刷新验证码");
        else if (info.text == "修改当前学期")
            operateTerm = teacherInfo->getCurrentTerm();

        bool available = operateTerm.isValid();
        if (info.text == "刷新验证码") available = true;
        button->setEnabled(available);
        if (!available) button->setToolTip("当前不可用");

        connect(button, &QToolButton::clicked, this, [=]() {
            if (info.text == "刷新验证码") {
                teacherInfo->refreshVerifyCode();
                teacherInfo->save();
                QMessageBox::information(this, "验证码已更新",
                                         QString("已更新验证码为：%1")
                                             .arg(teacherInfo->getverifycode()));
            } else {
                showSubPage(info.text, operateTerm);
            }
        });
        functionButtons.append(button);
        functionTerms.append(operateTerm);
        col++;
        if (col >= 3) { col = 0; row++; }
    }

    mainLayout->addLayout(grid);
    page->setLayout(mainLayout);
    return page;
}

void styleButton(QPushButton *btn, const QString &baseColor = "#4CAF50") {
    btn->setMinimumSize(160, 50);
    btn->setStyleSheet(QString(
                           "QPushButton {"
                           "  background-color: %1;"
                           "  color: white;"
                           "  border: none;"
                           "  border-radius: 10px;"
                           "  font-weight: bold;"
                           "  font-size: 16px;"
                           "  padding: 10px;"
                           "}"
                           "QPushButton:hover {"
                           "  background-color: #388E3C;"
                           // "  cursor: pointer;"
                           "}"
                           "QPushButton:disabled {"
                           "  background-color: #cccccc;"
                           "  color: #666666;"
                           "}"
                           ).arg(baseColor));
}
QWidget* TeacherWindow::createSubPage(const QString &pageName, const Term &operateTerm) {
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    QPushButton *backButton = new QPushButton("←", page);
    backButton->setFixedSize(36, 36);

    backButton->setStyleSheet(R"(
    QPushButton {
        background-color: #a7e6a5;
        border: 2px solid #59c959;
        border-radius: 18px;
        font-size: 18px;
        font-weight: bold;
        color: #2d7035;
    }

    QPushButton:hover {
        background-color: #91db91;
        border-color: #4fb54f;
    }

    QPushButton:pressed {
        background-color: #7dcd7d;
    }
    )");
    connect(backButton, &QPushButton::clicked, this, &TeacherWindow::showMainPage);
    layout->addWidget(backButton);
    layout->addStretch();
    if (pageName == "手工选课"){
        QLabel *info = new QLabel("请输入学号与课程索引号进行手工选课：", page);
        layout->addWidget(info);

        QLineEdit *idBox = new QLineEdit(page);
        idBox->setPlaceholderText("学号");
        layout->addWidget(idBox);

        QLineEdit *courseBox = new QLineEdit(page);
        courseBox->setPlaceholderText("课程索引号");
        layout->addWidget(courseBox);

        QPushButton *submitBtn = new QPushButton("确认选课", page);
        layout->addWidget(submitBtn);

        QLabel *result = new QLabel(page);
        result->setStyleSheet("color: red;");
        layout->addWidget(result);

        connect(submitBtn, &QPushButton::clicked, this, [=]() {
            QString userId = idBox->text();
            QString courseId = courseBox->text();

            if (userId.isEmpty() || courseId.isEmpty()) {
                result->setText("学号和课程索引不能为空！");
                return;
            }

            QFile userFile("users.json");
            if (!userFile.open(QIODevice::ReadOnly)) {
                result->setText("无法读取用户数据！");
                return;
            }
            QJsonDocument userDoc = QJsonDocument::fromJson(userFile.readAll());
            userFile.close();
            QJsonObject userRoot = userDoc.object();
            QJsonObject matchedUserObj;
            QString matchedKey;
            bool userFound = false;

            for (const QString &key : userRoot.keys()) {
                QJsonObject obj = userRoot[key].toObject();
                if (obj["index"].toString() == userId) {
                    matchedUserObj = obj;
                    matchedKey = key;
                    userFound = true;
                    break;
                }
            }

            if (!userFound) {
                result->setText("用户不存在！");
                return;
            }

            QVector<CourseInfo> All_courses = loadCoursesFromJsonFile(":/resources/resources/courses.json");

            bool courseFound = false;
            CourseInfo foundcourse;
            for (const CourseInfo &c : All_courses) {
                if (c.index == courseId) {
                    courseFound = true;
                    foundcourse = c;
                    break;
                }
            }

            if (!courseFound) {
                result->setText("课程不存在！");
                return;
            }

            // TODO: 冲突判断
            if (checkConflict(userId, courseId)) {
                result->setText("课程与已有课程冲突！");
                return;
            }

            QJsonObject userObj = userRoot[matchedKey].toObject();
            QJsonArray courseArray = userObj["currentCourses"].toArray();
            courseArray.append(foundcourse.toJson());
            userObj["currentCourses"] = courseArray;
            userRoot[matchedKey] = userObj;
            qDebug() << courseArray;
            QFile saveFile("users.json");
            if (!saveFile.open(QIODevice::WriteOnly)) {
                result->setText("保存失败！");
                return;
            }
            saveFile.write(QJsonDocument(userRoot).toJson(QJsonDocument::Indented));
            saveFile.close();

            result->setStyleSheet("color: green;");
            result->setText("手工选课成功！");
        });

        layout->addStretch();
    }
    else if (pageName == "修改当前学期"){
        QLabel *curLabel = new QLabel("当前学期：" + teacherInfo->getCurrentTerm().toString(), page);
        QLabel *enrollLabel = new QLabel("当前选课学期：" + teacherInfo->getEnrollmentTerm().toString(), page);
        QLabel *upcomingLabel = new QLabel("待开始选课学期：" + teacherInfo->getUpcomingTerm().toString(), page);

        layout->addWidget(curLabel);
        layout->addWidget(enrollLabel);
        layout->addWidget(upcomingLabel);
        layout->addSpacing(20);
        QPushButton *passTermBtn = new QPushButton("推进学期", page);
        styleButton(passTermBtn, "#4CAF50");

        QPushButton *endElectBtn = new QPushButton("选课结束", page);
        styleButton(endElectBtn, "#FF9800");

        QPushButton *startElectBtn = new QPushButton("开始选课", page);
        styleButton(startElectBtn, "#2196F3");
        connect(passTermBtn, &QPushButton::clicked, this, [=]() {
            Term next = teacherInfo->getCurrentTerm().nextTerm();
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "确认操作",
                QString("确定要将当前学期推进至 %1 吗？").arg(next.toString()),
                QMessageBox::Yes | QMessageBox::No
                );

            if (reply == QMessageBox::Yes) {
                teacherInfo->TermPass();
                teacherInfo->save();
                QMessageBox::information(this, "提示", "操作成功！");
                curLabel->setText("当前学期：" + teacherInfo->getCurrentTerm().toString());
                enrollLabel->setText("当前选课学期：" + teacherInfo->getEnrollmentTerm().toString());
                upcomingLabel->setText("待开始选课学期：" + teacherInfo->getUpcomingTerm().toString());

                if (!teacherInfo->getUpcomingTerm().isValid()) {
                    startElectBtn->setEnabled(false);
                    startElectBtn->setToolTip("当前不可用");
                }
                else {
                    startElectBtn->setEnabled(true);
                    startElectBtn->setToolTip("");
                }
                if (!teacherInfo->getEnrollmentTerm().isValid()) {
                    endElectBtn->setEnabled(false);
                    endElectBtn->setToolTip("当前不可用");
                }
                else {
                    endElectBtn->setEnabled(true);
                    endElectBtn->setToolTip("");
                }
                // showMainPage();
            }
        });
        layout->addWidget(passTermBtn);


        connect(endElectBtn, &QPushButton::clicked, this, [=]() {
            Term next = teacherInfo->getEnrollmentTerm();
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "确认操作",
                QString("确定要结束当前选课学期（%1）吗？").arg(next.toString()),
                QMessageBox::Yes | QMessageBox::No
                );

            if (reply == QMessageBox::Yes) {
                teacherInfo->ElectiveTermPass();
                teacherInfo->save();
                QMessageBox::information(this, "提示", "操作成功！");
                curLabel->setText("当前学期：" + teacherInfo->getCurrentTerm().toString());
                enrollLabel->setText("当前选课学期：" + teacherInfo->getEnrollmentTerm().toString());
                upcomingLabel->setText("待开始选课学期：" + teacherInfo->getUpcomingTerm().toString());

                if (!teacherInfo->getUpcomingTerm().isValid()) {
                    startElectBtn->setEnabled(false);
                    startElectBtn->setToolTip("当前不可用");
                }
                else {
                    startElectBtn->setEnabled(true);
                    startElectBtn->setToolTip("");
                }
                if (!teacherInfo->getEnrollmentTerm().isValid()) {
                    endElectBtn->setEnabled(false);
                    endElectBtn->setToolTip("当前不可用");
                }
                else {
                    endElectBtn->setEnabled(true);
                    endElectBtn->setToolTip("");
                }
                // showMainPage();
            }
        });
        layout->addWidget(endElectBtn);


        connect(startElectBtn, &QPushButton::clicked, this, [=]() {
            Term next = teacherInfo->getUpcomingTerm();
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "确认操作",
                QString("确定要开始选课学期（%1）吗？").arg(next.toString()),
                QMessageBox::Yes | QMessageBox::No
                );

            if (reply == QMessageBox::Yes) {
                teacherInfo->ElectiveTermCome();
                teacherInfo->save();
                QMessageBox::information(this, "提示", "操作成功！");
                curLabel->setText("当前学期：" + teacherInfo->getCurrentTerm().toString());
                enrollLabel->setText("当前选课学期：" + teacherInfo->getEnrollmentTerm().toString());
                upcomingLabel->setText("待开始选课学期：" + teacherInfo->getUpcomingTerm().toString());

                if (!teacherInfo->getUpcomingTerm().isValid()) {
                    startElectBtn->setEnabled(false);
                    startElectBtn->setToolTip("当前不可用");
                }
                else {
                    startElectBtn->setEnabled(true);
                    startElectBtn->setToolTip("");
                }
                if (!teacherInfo->getEnrollmentTerm().isValid()) {
                    endElectBtn->setEnabled(false);
                    endElectBtn->setToolTip("当前不可用");
                }
                else {
                    endElectBtn->setEnabled(true);
                    endElectBtn->setToolTip("");
                }
                // showMainPage();
            }
        });
        layout->addWidget(startElectBtn);

        if (!teacherInfo->getUpcomingTerm().isValid()) {
            startElectBtn->setEnabled(false);
            startElectBtn->setToolTip("当前不可用");
        }
        else {
            startElectBtn->setEnabled(true);
            startElectBtn->setToolTip("");
        }
        if (!teacherInfo->getEnrollmentTerm().isValid()) {
            endElectBtn->setEnabled(false);
            endElectBtn->setToolTip("当前不可用");
        }
        else {
            endElectBtn->setEnabled(true);
            endElectBtn->setToolTip("");
        }

        layout->addStretch();
    }
    // else if
    else{
        QLabel *label = new QLabel(
        QString("功能：%1\n操作学期：%2（待实现）")
            .arg(pageName)
            .arg(operateTerm.isValid() ? operateTerm.toString() : "无效"), page
        );
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        layout->addStretch();
    }
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
    refreshMainPage();
    stackedWidget->setCurrentIndex(0);
}
