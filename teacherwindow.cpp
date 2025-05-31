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
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QFormLayout>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QComboBox>
#include <QCompleter>
#include <QSpinBox>
#include "CourseInfo.h"
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
void TeacherWindow::refreshCourseTable() {
    if (!courseTable) return;
    courseTable->setRowCount(courses.size());

    for (int i = 0; i < courses.size(); ++i) {
        const CourseInfo &c = courses[i];
        auto setCenteredItem = [&](int col, const QString &text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            courseTable->setItem(i, col, item);
        };

        setCenteredItem(0, c.code);
        setCenteredItem(1, c.type);
        setCenteredItem(2, c.name);
        setCenteredItem(3, c.classNumber);
        setCenteredItem(4, c.teacherList.join(", "));
        setCenteredItem(5, c.week);
        setCenteredItem(6, c.timeList.join("；\n"));
        setCenteredItem(7, c.unit);
        setCenteredItem(8, c.score);
        setCenteredItem(9, c.info);

        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);
        checkItem->setTextAlignment(Qt::AlignCenter);
        courseTable->setItem(i, 10, checkItem);
    }
}
void TeacherWindow::importCoursesFromCSV() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择课程 CSV 文件", "", "CSV 文件 (*.csv)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "文件打开失败", "无法打开所选文件！");
        return;
    }

    QTextStream in(&file);
    // in.setCodec("UTF-8");
    int lineNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (++lineNum == 1) continue; // 跳过表头

        QStringList fields = line.split(QRegularExpression(",(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)")); // 支持引号包裹
        if (fields.size() <10) continue;

        CourseInfo course;
        course.index = QString::number(courses.size() + courses.size() + 1);
        course.code = fields[1].trimmed();
        course.name = fields[2].trimmed();
        course.unit = fields[3].trimmed();
        course.type = fields[4].trimmed();
        course.classNumber = fields[5].trimmed();
        course.score = fields[6].trimmed();
        course.week = fields[7].trimmed();

        QString rawTime = fields[8].trimmed();
        course.timeList = rawTime.split("\n", Qt::SkipEmptyParts);

        QString rawTeachers = fields[9].trimmed();
        course.teacherList = rawTeachers.split(QRegularExpression("[，,\\s]+"), Qt::SkipEmptyParts);

        course.info = fields.size()>=11 ? fields[10].trimmed() : "";
        course.firstClass = "无";
        course.chineseIntro = "暂无介绍";
        course.englishIntro = "No introduction.";

        // 更新JSON对象（以便toJson()可用）
        QJsonObject obj;
        obj["index"] = course.index;
        obj["code"] = course.code;
        obj["type"] = course.type;
        obj["name"] = course.name;
        obj["class"] = course.classNumber;
        obj["teacher"] = QJsonArray::fromStringList(course.teacherList);
        obj["week"] = course.week;
        obj["time"] = QJsonArray::fromStringList(course.timeList);
        obj["unit"] = course.unit;
        obj["score"] = course.score;
        obj["info"] = course.info;
        obj["first-class"] = course.firstClass;
        obj["chinese-intro"] = course.chineseIntro;
        obj["english-intro"] = course.englishIntro;
        course.obj = obj;

        courses.append(course);
    }

    file.close();
    refreshCourseTable();
    saveCoursesToFile();
    QMessageBox::information(this, "导入完成", "课程数据已成功导入！");
}
void TeacherWindow::saveCoursesToFile() {
    QJsonArray arr;
    for (const auto &c : courses)
        arr.append(c.toJson());

    QFile file(currentCourseFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "保存失败", "无法保存课程数据到文件！");
        return;
    }
    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
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
    // layout->addStretch();
    if (pageName == "编辑课程列表") {

        currentCourseFilePath = QString("course_%1.json").arg(operateTerm.toString());
        loadCoursesFromFile();
        refreshCourseTable();
        QLineEdit *searchBox = new QLineEdit(page);
        searchBox->setPlaceholderText("搜索课程号/名称...");
        layout->addWidget(searchBox);

        QTableWidget *table = new QTableWidget(page);
        table->setColumnCount(11);
        table->setHorizontalHeaderLabels({"课程号", "类型", "课程名", "班号", "教师", "上课周", "时间", "开课单位", "学分", "备注", "选择"});
        table->horizontalHeader()->setStretchLastSection(true);
        table->setColumnWidth(10, 40);
        courseTable = table;
        refreshCourseTable();
        layout->addWidget(table);

        QPushButton *importBtn = new QPushButton("从csv文件中导入", page);
        QPushButton *addBtn = new QPushButton("手动添加", page);
        QPushButton *editBtn = new QPushButton("编辑课程", page);
        QPushButton *deleteBtn = new QPushButton("删除课程", page);

        editBtn->setEnabled(false);
        deleteBtn->setEnabled(false);

        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->addWidget(importBtn);
        btnLayout->addWidget(addBtn);
        btnLayout->addWidget(editBtn);
        btnLayout->addWidget(deleteBtn);
        layout->addLayout(btnLayout);

        connect(table, &QTableWidget::itemChanged, this, [=]() {
            int selected = 0;
            for (int i = 0; i < table->rowCount(); ++i) {
                QTableWidgetItem *item = table->item(i, 10);
                if (item && item->checkState() == Qt::Checked) ++selected;
            }
            addBtn->setEnabled(selected == 0);
            editBtn->setEnabled(selected == 1);
            deleteBtn->setEnabled(selected >= 1);
        });

        connect(searchBox, &QLineEdit::textChanged, this, [=]() {
            for (int i = 0; i < table->rowCount(); ++i) {
                bool match = false;
                for (int j : {0, 2}) {
                    auto *item = table->item(i, j);
                    if (item && item->text().contains(searchBox->text(), Qt::CaseInsensitive)) {
                        match = true;
                        break;
                    }
                }
                table->setRowHidden(i, !match);
            }
        });

        // TODO: connect importBtn, addBtn, editBtn, deleteBtn with actual logic
        connect(importBtn, &QPushButton::clicked, this, &TeacherWindow::importCoursesFromCSV);
        connect(addBtn, &QPushButton::clicked, this, [=]() {
            QDialog dialog(this);
            dialog.setWindowTitle("添加课程");
            QFormLayout form(&dialog);

            QLineEdit *codeEdit = new QLineEdit(&dialog);
            QLineEdit *nameEdit = new QLineEdit(&dialog);
            QStringList colleges = {
                "数学科学学院",
                "物理学院",
                "化学与分子工程学院",
                "生命科学学院",
                "地球与空间科学学院",
                "心理与认知科学学院",
                "新闻与传播学院",
                "中国语言文学系",
                "历史学系",
                "考古文博学院",
                "哲学系",
                "国际关系学院",
                "经济学院",
                "光华管理学院",
                "法学院",
                "信息管理系",
                "社会学系",
                "政府管理学院",
                "英语语言文学系",
                "外国语学院",
                "马克思主义学院",
                "体育教研部",
                "艺术学院",
                "元培学院",
                "信息科学技术学院",
                "国家发展研究院",
                "工学院",
                "城市与环境学院",
                "环境科学与工程学院"
            };

            QComboBox *collegeBox = new QComboBox(this);
            collegeBox->setEditable(true);
            collegeBox->addItems(colleges);
            collegeBox->setCompleter(new QCompleter(colleges, this));
            // QLineEdit *unitEdit = new QLineEdit(&dialog);
            // QLineEdit *typeEdit = new QLineEdit(&dialog);
            QComboBox *typeEdit = new QComboBox(&dialog);
            typeEdit->addItems({"毕业论文/设计",
                              "大学英语",
                              "军事理论",
                              "理科生必修",
                              "全校公选课",
                              "实习实践",
                              "双学位",
                              "思想政治",
                              "体育",
                              "通选课",
                              "文科生必修",
                              "专业必修",
                              "专业任选",
                              "专业限选"});
            QLineEdit *classEdit = new QLineEdit(&dialog);
            QSpinBox *scoreEdit = new QSpinBox(&dialog);
            scoreEdit->setRange(0, 100);
            QSpinBox *weekStartEdit = new QSpinBox(&dialog);
            QSpinBox *weekEndEdit = new QSpinBox(&dialog);
            weekStartEdit->setRange(1, 16);
            weekEndEdit->setRange(1, 16);
            // QLineEdit *timeEdit = new QLineEdit(&dialog);
            // QLineEdit *teacherEdit = new QLineEdit(&dialog);
            QLineEdit *infoEdit = new QLineEdit(&dialog);
            QLineEdit *firstClassEdit = new QLineEdit(&dialog);
            QPlainTextEdit *chineseIntroEdit = new QPlainTextEdit(&dialog);
            QPlainTextEdit *englishIntroEdit = new QPlainTextEdit(&dialog);

            // 教师
            QLineEdit *teacherInput = new QLineEdit;
            QPushButton *addTeacherBtn = new QPushButton("➕");
            QVBoxLayout *teacherListLayout = new QVBoxLayout;
            QHBoxLayout *teacherInputLayout = new QHBoxLayout;
            teacherInputLayout->addWidget(teacherInput);
            teacherInputLayout->addWidget(addTeacherBtn);
            QStringList teacherList;
            connect(addTeacherBtn, &QPushButton::clicked, &dialog, [&]() {
                QString name = teacherInput->text().trimmed();
                if (!name.isEmpty()) {
                    teacherList.append(name);
                    QLabel *lbl = new QLabel("👤 " + name);
                    QPushButton *delBtn = new QPushButton("❌");
                    delBtn->setFixedSize(20, 20);
                    QHBoxLayout *h = new QHBoxLayout;
                    h->addWidget(lbl);
                    h->addWidget(delBtn);
                    QWidget *container = new QWidget;
                    container->setLayout(h);
                    teacherListLayout->addWidget(container);
                    connect(delBtn, &QPushButton::clicked, &dialog, [=, &teacherList]() {
                        teacherList.removeAll(name);
                        container->deleteLater();
                    });
                    teacherInput->clear();
                }
            });

            // 上课时间
            QComboBox *dayBox = new QComboBox;
            dayBox->addItems({"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"});
            QSpinBox *startPeriod = new QSpinBox;
            QSpinBox *endPeriod = new QSpinBox;
            startPeriod->setRange(1, 13);
            endPeriod->setRange(1, 13);
            QComboBox *weekTypeBox = new QComboBox;
            weekTypeBox->addItems({"每周", "单周", "双周"});
            QPushButton *addTimeBtn = new QPushButton("➕");
            QVBoxLayout *timeListLayout = new QVBoxLayout;
            QHBoxLayout *timeInputLayout = new QHBoxLayout;
            timeInputLayout->addWidget(dayBox);
            timeInputLayout->addWidget(new QLabel("第"));
            timeInputLayout->addWidget(startPeriod);
            timeInputLayout->addWidget(new QLabel("节-第"));
            timeInputLayout->addWidget(endPeriod);
            timeInputLayout->addWidget(new QLabel("节"));
            timeInputLayout->addWidget(weekTypeBox);
            timeInputLayout->addWidget(addTimeBtn);
            QStringList timeList;
            connect(addTimeBtn, &QPushButton::clicked, &dialog, [&]() {
                int a = startPeriod->value(), b = endPeriod->value();
                if (a > b) {
                    QMessageBox::warning(nullptr, "错误", "起始节必须小于等于结束节！");
                    return;
                }
                QString t = QString("%1(第%2节-第%3节)%4")
                                .arg(dayBox->currentText())
                                .arg(a).arg(b)
                                .arg(weekTypeBox->currentText() == "每周" ? "" :
                                         weekTypeBox->currentText() == "单周" ? "（单）" : "（双）");
                timeList.append(t);
                QLabel *lbl = new QLabel("🕓 " + t);
                QPushButton *delBtn = new QPushButton("❌");
                delBtn->setFixedSize(20, 20);
                QHBoxLayout *h = new QHBoxLayout;
                h->addWidget(lbl);
                h->addWidget(delBtn);
                QWidget *container = new QWidget;
                container->setLayout(h);
                timeListLayout->addWidget(container);
                connect(delBtn, &QPushButton::clicked, &dialog, [=, &timeList]() {
                    timeList.removeAll(t);
                    container->deleteLater();
                });
            });

            form.addRow("课程号：", codeEdit);
            form.addRow("课程名：", nameEdit);
            form.addRow("开课单位：", collegeBox);
            form.addRow("课程类型：", typeEdit);
            form.addRow("班级：", classEdit);
            form.addRow("学分：", scoreEdit);
            QHBoxLayout *weekLayout = new QHBoxLayout;
            weekLayout->addWidget(weekStartEdit);
            weekLayout->addWidget(new QLabel(" - "));
            weekLayout->addWidget(weekEndEdit);
            form.addRow("上课周（1~16）：", weekLayout);
            form.addRow("教师：", teacherInputLayout);
            form.addRow("", teacherListLayout);
            form.addRow("上课时间：", timeInputLayout);
            form.addRow("", timeListLayout);
            form.addRow("备注：", infoEdit);
            form.addRow("先修课程：", firstClassEdit);
            form.addRow("中文简介：", chineseIntroEdit);
            form.addRow("英文简介：", englishIntroEdit);

            QPushButton *okBtn = new QPushButton("确认", &dialog);
            form.addWidget(okBtn);
            connect(okBtn, &QPushButton::clicked, &dialog, [&]() {
                if (weekStartEdit->value() > weekEndEdit->value()) {
                    QMessageBox::warning(&dialog, "输入错误", "起始周必须小于等于结束周！");
                    return;
                }
                dialog.accept();
            });

            if (dialog.exec() == QDialog::Accepted) {
                int start = weekStartEdit->value();
                int end = weekEndEdit->value();
                // if (start > end) {
                //     QMessageBox::warning(this, "输入错误", "起始周必须小于等于结束周！");
                //     return;
                // }
                CourseInfo c;
                c.index = QString::number(courses.size());
                c.code = codeEdit->text();
                c.name = nameEdit->text();
                c.unit = collegeBox->currentText();
                c.type = typeEdit->currentText();
                c.classNumber = classEdit->text();
                c.score = QString::number(scoreEdit->value());
                c.week = QString("%1-%2").arg(start).arg(end);
                c.teacherList = teacherList;
                c.timeList = timeList;
                c.info = infoEdit->text();
                c.firstClass = firstClassEdit->text();
                c.chineseIntro = chineseIntroEdit->toPlainText();
                c.englishIntro = englishIntroEdit->toPlainText();

                QJsonObject obj;
                obj["index"] = c.index;
                obj["code"] = c.code;
                obj["name"] = c.name;
                obj["unit"] = c.unit;
                obj["type"] = c.type;
                obj["class"] = c.classNumber;
                obj["score"] = c.score;
                obj["week"] = c.week;
                obj["time"] = QJsonArray::fromStringList(c.timeList);
                obj["teacher"] = QJsonArray::fromStringList(c.teacherList);
                obj["info"] = c.info;
                obj["first-class"] = c.firstClass;
                obj["chinese-intro"] = c.chineseIntro;
                obj["english-intro"] = c.englishIntro;
                c.obj = obj;

                courses.append(c);
                refreshCourseTable();
                saveCoursesToFile();

                QMessageBox::information(this, "成功", "课程已添加并保存。");
            }
        });
        connect(editBtn, &QPushButton::clicked, this, [=]() {
            int targetRow = -1;
            for (int i = 0; i < courseTable->rowCount(); ++i) {
                auto *item = courseTable->item(i, 10);
                if (item && item->checkState() == Qt::Checked) {
                    targetRow = i;
                    break;
                }
            }
            if (targetRow != -1) {
                CourseInfo &c = courses[targetRow];
                QDialog dialog(this);
                dialog.setWindowTitle("编辑课程：" + c.name);
                QFormLayout form(&dialog);

                QLineEdit *codeEdit = new QLineEdit(c.code, &dialog);
                QLineEdit *nameEdit = new QLineEdit(c.name, &dialog);
                QComboBox *collegeBox = new QComboBox(&dialog);
                QStringList colleges = {"数学科学学院", "物理学院", "化学与分子工程学院", "生命科学学院", "地球与空间科学学院", "心理与认知科学学院", "新闻与传播学院", "中国语言文学系", "历史学系", "考古文博学院", "哲学系", "国际关系学院", "经济学院", "光华管理学院", "法学院", "信息管理系", "社会学系", "政府管理学院", "英语语言文学系", "外国语学院", "马克思主义学院", "体育教研部", "艺术学院", "元培学院", "信息科学技术学院", "国家发展研究院", "工学院", "城市与环境学院", "环境科学与工程学院"};
                collegeBox->addItems(colleges);
                collegeBox->setCurrentText(c.unit);
                collegeBox->setEditable(true);

                QComboBox *typeEdit = new QComboBox(&dialog);
                typeEdit->addItems({"毕业论文/设计", "大学英语", "军事理论", "理科生必修", "全校公选课", "实习实践", "双学位", "思想政治", "体育", "通选课", "文科生必修", "专业必修", "专业任选", "专业限选"});
                typeEdit->setCurrentText(c.type);

                QLineEdit *classEdit = new QLineEdit(c.classNumber, &dialog);
                QSpinBox *scoreEdit = new QSpinBox(&dialog);
                scoreEdit->setRange(0, 100);
                scoreEdit->setValue(c.score.toInt());

                QSpinBox *weekStartEdit = new QSpinBox(&dialog);
                QSpinBox *weekEndEdit = new QSpinBox(&dialog);
                weekStartEdit->setRange(1, 16);
                weekEndEdit->setRange(1, 16);
                QStringList weekParts = c.week.split("-");
                if (weekParts.size() == 2) {
                    weekStartEdit->setValue(weekParts[0].toInt());
                    weekEndEdit->setValue(weekParts[1].toInt());
                }

                QLineEdit *infoEdit = new QLineEdit(c.info, &dialog);
                QLineEdit *firstClassEdit = new QLineEdit(c.firstClass, &dialog);
                QPlainTextEdit *chineseIntroEdit = new QPlainTextEdit(c.chineseIntro, &dialog);
                QPlainTextEdit *englishIntroEdit = new QPlainTextEdit(c.englishIntro, &dialog);

                QLineEdit *teacherInput = new QLineEdit;
                QPushButton *addTeacherBtn = new QPushButton("➕");
                QVBoxLayout *teacherListLayout = new QVBoxLayout;
                QHBoxLayout *teacherInputLayout = new QHBoxLayout;
                teacherInputLayout->addWidget(teacherInput);
                teacherInputLayout->addWidget(addTeacherBtn);
                QStringList teacherList = c.teacherList;
                for (const auto &t : teacherList) {
                    QLabel *lbl = new QLabel("👤 " + t);
                    QPushButton *delBtn = new QPushButton("❌");
                    delBtn->setFixedSize(20, 20);
                    QHBoxLayout *h = new QHBoxLayout;
                    h->addWidget(lbl);
                    h->addWidget(delBtn);
                    QWidget *container = new QWidget;
                    container->setLayout(h);
                    teacherListLayout->addWidget(container);
                    connect(delBtn, &QPushButton::clicked, &dialog, [=, &teacherList]() {
                        teacherList.removeAll(t);
                        container->deleteLater();
                    });
                }
                connect(addTeacherBtn, &QPushButton::clicked, &dialog, [&]() {
                    QString name = teacherInput->text().trimmed();
                    if (!name.isEmpty()) {
                        teacherList.append(name);
                        QLabel *lbl = new QLabel("👤 " + name);
                        QPushButton *delBtn = new QPushButton("❌");
                        delBtn->setFixedSize(20, 20);
                        QHBoxLayout *h = new QHBoxLayout;
                        h->addWidget(lbl);
                        h->addWidget(delBtn);
                        QWidget *container = new QWidget;
                        container->setLayout(h);
                        teacherListLayout->addWidget(container);
                        connect(delBtn, &QPushButton::clicked, &dialog, [=, &teacherList]() {
                            teacherList.removeAll(name);
                            container->deleteLater();
                        });
                        teacherInput->clear();
                    }
                });

                QComboBox *dayBox = new QComboBox;
                dayBox->addItems({"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"});
                QSpinBox *startPeriod = new QSpinBox;
                QSpinBox *endPeriod = new QSpinBox;
                startPeriod->setRange(1, 13);
                endPeriod->setRange(1, 13);
                QComboBox *weekTypeBox = new QComboBox;
                weekTypeBox->addItems({"每周", "单周", "双周"});
                QPushButton *addTimeBtn = new QPushButton("➕");
                QVBoxLayout *timeListLayout = new QVBoxLayout;
                QHBoxLayout *timeInputLayout = new QHBoxLayout;
                timeInputLayout->addWidget(dayBox);
                timeInputLayout->addWidget(new QLabel("第"));
                timeInputLayout->addWidget(startPeriod);
                timeInputLayout->addWidget(new QLabel("节-第"));
                timeInputLayout->addWidget(endPeriod);
                timeInputLayout->addWidget(new QLabel("节"));
                timeInputLayout->addWidget(weekTypeBox);
                timeInputLayout->addWidget(addTimeBtn);
                QStringList timeList = c.timeList;
                for (const auto &t : timeList) {
                    QLabel *lbl = new QLabel("🕓 " + t);
                    QPushButton *delBtn = new QPushButton("❌");
                    delBtn->setFixedSize(20, 20);
                    QHBoxLayout *h = new QHBoxLayout;
                    h->addWidget(lbl);
                    h->addWidget(delBtn);
                    QWidget *container = new QWidget;
                    container->setLayout(h);
                    timeListLayout->addWidget(container);
                    connect(delBtn, &QPushButton::clicked, &dialog, [=, &timeList]() {
                        timeList.removeAll(t);
                        container->deleteLater();
                    });
                }
                connect(addTimeBtn, &QPushButton::clicked, &dialog, [&]() {
                    int a = startPeriod->value(), b = endPeriod->value();
                    if (a > b) {
                        QMessageBox::warning(nullptr, "错误", "起始节必须小于等于结束节！");
                        return;
                    }
                    QString t = QString("%1(第%2节-第%3节)%4")
                                    .arg(dayBox->currentText())
                                    .arg(a).arg(b)
                                    .arg(weekTypeBox->currentText() == "每周" ? "" :
                                             weekTypeBox->currentText() == "单周" ? "（单）" : "（双）");
                    timeList.append(t);
                    QLabel *lbl = new QLabel("🕓 " + t);
                    QPushButton *delBtn = new QPushButton("❌");
                    delBtn->setFixedSize(20, 20);
                    QHBoxLayout *h = new QHBoxLayout;
                    h->addWidget(lbl);
                    h->addWidget(delBtn);
                    QWidget *container = new QWidget;
                    container->setLayout(h);
                    timeListLayout->addWidget(container);
                    connect(delBtn, &QPushButton::clicked, &dialog, [=, &timeList]() {
                        timeList.removeAll(t);
                        container->deleteLater();
                    });
                });

                form.addRow("课程号：", codeEdit);
                form.addRow("课程名：", nameEdit);
                form.addRow("开课单位：", collegeBox);
                form.addRow("课程类型：", typeEdit);
                form.addRow("班级：", classEdit);
                form.addRow("学分：", scoreEdit);
                QHBoxLayout *weekLayout = new QHBoxLayout;
                weekLayout->addWidget(weekStartEdit);
                weekLayout->addWidget(new QLabel(" - "));
                weekLayout->addWidget(weekEndEdit);
                form.addRow("上课周：", weekLayout);
                form.addRow("教师：", teacherInputLayout);
                form.addRow("", teacherListLayout);
                form.addRow("上课时间：", timeInputLayout);
                form.addRow("", timeListLayout);
                form.addRow("备注：", infoEdit);
                form.addRow("先修课程：", firstClassEdit);
                form.addRow("中文简介：", chineseIntroEdit);
                form.addRow("英文简介：", englishIntroEdit);

                QPushButton *okBtn = new QPushButton("保存", &dialog);
                form.addWidget(okBtn);
                connect(okBtn, &QPushButton::clicked, &dialog, [&]() {
                    if (weekStartEdit->value() > weekEndEdit->value()) {
                        QMessageBox::warning(&dialog, "输入错误", "起始周必须小于等于结束周！");
                        return;
                    }
                    dialog.accept();
                });

                if (dialog.exec() == QDialog::Accepted) {
                    // if (weekStartEdit->value() > weekEndEdit->value()) {
                    //     QMessageBox::warning(this, "错误", "起始周不能大于结束周！");
                    //     return;
                    // }
                    c.code = codeEdit->text();
                    c.name = nameEdit->text();
                    c.unit = collegeBox->currentText();
                    c.type = typeEdit->currentText();
                    c.classNumber = classEdit->text();
                    c.score = QString::number(scoreEdit->value());
                    c.week = QString("%1-%2").arg(weekStartEdit->value()).arg(weekEndEdit->value());
                    c.teacherList = teacherList;
                    c.timeList = timeList;
                    c.info = infoEdit->text();
                    c.firstClass = firstClassEdit->text();
                    c.chineseIntro = chineseIntroEdit->toPlainText();
                    c.englishIntro = englishIntroEdit->toPlainText();

                    QJsonObject obj;
                    obj["index"] = c.index;
                    obj["code"] = c.code;
                    obj["name"] = c.name;
                    obj["unit"] = c.unit;
                    obj["type"] = c.type;
                    obj["class"] = c.classNumber;
                    obj["score"] = c.score;
                    obj["week"] = c.week;
                    obj["time"] = QJsonArray::fromStringList(c.timeList);
                    obj["teacher"] = QJsonArray::fromStringList(c.teacherList);
                    obj["info"] = c.info;
                    obj["first-class"] = c.firstClass;
                    obj["chinese-intro"] = c.chineseIntro;
                    obj["english-intro"] = c.englishIntro;
                    c.obj = obj;

                    refreshCourseTable();
                    saveCoursesToFile();
                    QMessageBox::information(this, "成功", "课程信息已更新。");
                }
            }
        });

        connect(deleteBtn, &QPushButton::clicked, this, [=]() {
            QVector<int> selected;
            for (int i = 0; i < courseTable->rowCount(); ++i) {
                auto *item = courseTable->item(i, 10);
                if (item && item->checkState() == Qt::Checked) {
                    selected.append(i);
                }
            }
            if (selected.isEmpty()) return;

            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "删除课程", "确定要删除选中的课程吗？", QMessageBox::Yes | QMessageBox::No
                );
            if (reply == QMessageBox::Yes) {
                std::sort(selected.begin(), selected.end(), std::greater<>());
                for (int i : selected) {
                    courses.remove(i);
                    courseTable->removeRow(i);
                }
                for (int i = 0; i < courses.size(); ++i) {
                    courses[i].index = QString::number(i);
                }
                refreshCourseTable();
                saveCoursesToFile();
                QMessageBox::information(this, "提示", "课程已删除。");
            }
        });
    }
    else if (pageName == "手工选课"){
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
    if (pageName == "编辑课程列表") this->resize(1130, 700);
    if (!subPages.contains(key)) {
        QWidget *subPage = createSubPage(pageName, operateTerm);
        subPages[key] = subPage;
        stackedWidget->addWidget(subPage);
    }
    stackedWidget->setCurrentWidget(subPages[key]);
}

void TeacherWindow::showMainPage() {
    resize(300, 300);
    refreshMainPage();
    stackedWidget->setCurrentIndex(0);
}
