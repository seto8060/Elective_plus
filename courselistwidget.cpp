#include "CourseListWidget.h"
#include<QVBoxLayout>
#include<QLabel>
#include<QHeaderView>

CourseListWidget::CourseListWidget(QWidget *parent,int type) : QWidget(parent) {
    table = new QTableWidget(this);
    QLabel *titleLabel = nullptr;
    if (type == 0) titleLabel = new QLabel("选课列表", this);
    if (type == 2) titleLabel = new QLabel("收藏夹",this);
    titleLabel->setStyleSheet("font-size: 40px; font-weight: bold; color: #333;");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *footerNote = new QLabel("备注：投点……待做", this);
    footerNote->setStyleSheet("color: red; font-size: 12px; padding: 6px;");
    footerNote->setAlignment(Qt::AlignLeft);
    switchButton = new QPushButton("切换到课程列表", this);
    switchButton->setText("⇄");
    switchButton->setFixedSize(36, 36);

    switchButton->setStyleSheet(R"(
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
    QHBoxLayout *topLayout = new QHBoxLayout;
    if (type!=2){
        // QHBoxLayout *topLayout = new QHBoxLayout;
        topLayout->addWidget(switchButton);
        topLayout->addStretch();
    }
    else {
        switchButton->hide();
    }
    table->setColumnCount(12);
    table->setHorizontalHeaderLabels({"课程号", "类型","课程名", "班号","教师", "上课周","时间", "开课单位","学分","备注","已选/限选","操作"});
    table->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->setSpacing(0);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->addWidget(titleLabel);
    tableLayout->addWidget(table);
    tableLayout->addWidget(footerNote);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    if (type!=2) mainLayout->addLayout(topLayout);
    mainLayout->addLayout(tableLayout);
    setLayout(mainLayout);

    connect(switchButton, &QPushButton::clicked, this, [=]() {
        emit requestSwitchToTable();
    });
}

void CourseListWidget::setCourses(const QVector<CourseInfo> &courses,int type) {
    table->setRowCount(courses.size());

    for (int i = 0; i < courses.size(); ++i) {
        const CourseInfo &c = courses[i];
        auto setCenteredItem = [=](int col, const QString &text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, col, item);
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
        setCenteredItem(10, QString::number(c.Now_person) + "/" + QString::number(c.Max_person));
    }
    table->setColumnWidth(6, 160);
    table->setColumnWidth(3, 40);
    table->setColumnWidth(8, 40);
    table->setColumnWidth(5, 60);
    table->setColumnWidth(10, 80);
    table->setColumnWidth(11, 70);
    table->setWordWrap(true);
    table->resizeRowsToContents();
    int rowCount = table->rowCount();
    int totalHeight = table->horizontalHeader()->height();

    for (int i = 0; i < rowCount; ++i) {
        totalHeight += table->rowHeight(i);
    }
    totalHeight += 10;
    table->setFixedHeight(totalHeight);
    // table->setStyleSheet(R"(
    // QTableWidget {
    //     background-color: #f7f7f7;
    //     gridline-color: #ccc;
    //     font-size: 14px;
    // }

    // QHeaderView::section {
    //     background-color: #e0e0e0;
    //     padding: 4px;
    //     border: none;
    //     font-weight: bold;
    // }

    // QTableWidget::item {
    //     padding: 5px;
    // }

    // QTableWidget::item:selected {
    //     background-color: #cfe8fc;
    // }
    // )");
}
