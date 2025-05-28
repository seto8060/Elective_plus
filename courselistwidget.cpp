#include "CourseListWidget.h"
#include "userinfo.h"
#include <QMessageBox>
#include<QVBoxLayout>
#include<QLabel>
#include<QHeaderView>
#include<QSpinBox>
double estimateAverageVoteWeight(double omega) {
    return std::max(0.0, 46.0 * std::tanh(omega - 1.0));
}

double estimateSelectionProbability(int P_i, int N, int K) {
    if (K == 0) return 0.0;
    double omega = static_cast<double>(N) / K;
    double avg_other = estimateAverageVoteWeight(omega);
    double self_weight = P_i + 1;
    double total_weight = self_weight + (N - 1) * avg_other;
    double prob_not_selected = pow(1.0 - self_weight / total_weight, K);
    return 1.0 - prob_not_selected;
}
CourseListWidget::CourseListWidget(QWidget *parent,int type,UserInfo *userinfo,const QString& semester) : QWidget(parent) {
    //type = 0: CourseList; type = 1: HistoryList; type = 2: LoveList
    table = new QTableWidget(this);
    QLabel *titleLabel = nullptr;
    if (type == 0) titleLabel = new QLabel("选课列表", this);
    if (type == 1) titleLabel = new QLabel(semester+"选课情况", this);
    if (type == 2) titleLabel = new QLabel("收藏夹",this);
    titleLabel->setStyleSheet("font-size: 40px; font-weight: bold; color: #333;");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    int Sum_score = 0;double Drop_score = 0;
    for (auto& x:userinfo->getCurrentCourses()) Sum_score += x.score.toInt(), Drop_score += x.score.toDouble() * (1 - estimateSelectionProbability(
                                                                                    userinfo->getPointForCourse(x.code),x.Now_person + 1,x.Max_person));
    if (type == 0){

        footerNote = new QLabel("已选学分数："+QString::number(Sum_score)+" 剩余点数："+QString::number(userinfo->getRemainingPoints())+
                                    "   预计未中签学分（仅供参考）:"+QString::number(Drop_score, 'f', 2), this);
        footerNote->setStyleSheet("color: red; font-size: 12px; padding: 6px;");
        footerNote->setAlignment(Qt::AlignLeft);
    }
    else footerNote = new QLabel("");
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
    if (type!=1){
        table->setColumnCount(12);
        table->setHorizontalHeaderLabels({"课程号", "类型","课程名", "班号","教师", "上课周","时间", "开课单位","学分","备注","已选/限选","操作"});
    }
    else {
        table->setColumnCount(11);
        table->setHorizontalHeaderLabels({"课程号", "类型","课程名", "班号","教师", "上课周","时间", "开课单位","学分","备注","操作"});
    }
    table->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->setSpacing(0);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->addWidget(titleLabel);
    tableLayout->addWidget(table);
    // if (type == 0)
        tableLayout->addWidget(footerNote);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    if (type!=2) mainLayout->addLayout(topLayout);
    mainLayout->addLayout(tableLayout);
    setLayout(mainLayout);

    connect(switchButton, &QPushButton::clicked, this, [=]() {
        emit requestSwitchToTable();
    });
}

void CourseListWidget::setCourses(const QVector<CourseInfo> &courses,int type,UserInfo *userinfo) {
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
        if (type!=1) setCenteredItem(10, QString::number(c.Now_person) + "/" + QString::number(c.Max_person));
        else /*添加测评*/;

        if (type == 0){
            QWidget *cellWidget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(cellWidget);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(4);

            QSpinBox *pointBox = new QSpinBox();
            pointBox->setRange(0, 99);
            pointBox->setValue(userinfo->getPointForCourse(c.code));
            pointBox->setFixedWidth(70);
            pointBox->setAlignment(Qt::AlignCenter);

            pointBox->setMaximum(userinfo->getRemainingPoints());

            QPushButton *voteButton = new QPushButton("投点");
            voteButton->setFixedSize(50, 24);
            voteButton->setStyleSheet("QPushButton { font-size: 12px; padding: 2px; }");

            QLabel *infoIcon = new QLabel();
            infoIcon->setPixmap(QPixmap(":/resources/icon/warning.svg").scaled(16, 16));
            infoIcon->setToolTip(QString("当前中签概率估计为：%1%")
                                     .arg(QString::number(estimateSelectionProbability(
                                                              pointBox->value(), c.Now_person + 1, c.Max_person) * 100, 'f', 1)));

            layout->addWidget(pointBox);
            layout->addWidget(voteButton);
            layout->addWidget(infoIcon);
            layout->setAlignment(Qt::AlignCenter);
            cellWidget->setLayout(layout);
            table->setCellWidget(i, 11, cellWidget);

            // connect(pointBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val) {


            // });
            connect(voteButton, &QPushButton::clicked, this, [=]() mutable {
                int points = pointBox->value(), Now_points = userinfo->getPointForCourse(c.code);
                // qDebug() << points << ' ' << userinfo->getRemainingPoints();
                userinfo->setPointForCourse(c.code, points);
                if (0 > userinfo->getRemainingPoints()) {
                    QMessageBox::warning(this, "投点失败", "投点数超过剩余点数！");
                    userinfo->setPointForCourse(c.code, Now_points);
                    pointBox->setValue(userinfo->getPointForCourse(c.code));
                    return;
                }
                // userinfo->setPointForCourse(c.code, points);
                infoIcon->setToolTip(QString("当前中签概率估计为：%1%")
                                         .arg(QString::number(estimateSelectionProbability(
                                                                  pointBox->value(), c.Now_person + 1, c.Max_person) * 100, 'f', 1)));
                // voteButton->setEnabled(false);
                // pointBox->setEnabled(false);
                QMessageBox::information(this, "投点成功", QString("已为课程 [%1] 投入 %2 点").arg(c.name).arg(points));
                int Sum_score = 0;double Drop_score = 0;
                for (auto& x:userinfo->getCurrentCourses()) Sum_score += x.score.toInt(), Drop_score += x.score.toDouble() * (1 - estimateSelectionProbability(
                                                                    userinfo->getPointForCourse(x.code),x.Now_person + 1,x.Max_person));
                footerNote->setText("已选学分数：" + QString::number(Sum_score) +
                                    " 剩余点数：" + QString::number(userinfo->getRemainingPoints())+
                                    "   预计未中签学分（仅供参考）:"+QString::number(Drop_score, 'f', 2));
            });
        }
        // setCenteredItem(11,)
    }
    table->setColumnWidth(6, 160);
    table->setColumnWidth(3, 40);
    table->setColumnWidth(8, 40);
    table->setColumnWidth(5, 60);
    table->setColumnWidth(10, 80);
    table->setColumnWidth(11, 170);
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
