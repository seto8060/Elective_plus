#include "CourseListWidget.h"
#include "userinfo.h"
#include "teacherinfo.h"
#include "commentloader.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QMessageBox>
#include<QVBoxLayout>
#include<QLabel>
#include<QHeaderView>
#include<QSpinBox>
#include<QFormLayout>
#include<QLineEdit>
#include <QPlainTextEdit>
#include <QInputDialog>
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
CourseListWidget::CourseListWidget(QWidget *parent,int type,UserInfo *userinfo,const QString& semester,QVector<courseComment> *allcoursecomment) : QWidget(parent),semester(semester),m_allCourseComments(allcoursecomment) {
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
            if (text == "未选上") item->setForeground(Qt::red);
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
        else {
            QPushButton *reviewButton = new QPushButton("课程评价");
            reviewButton->setFixedSize(80, 24);
            reviewButton->setStyleSheet("QPushButton { font-size: 12px; padding: 2px; background-color: #f0f0f0;}QPushButton:hover { background-color: #e0e0e0;}");
            
            QWidget *cellWidget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(cellWidget);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(4);
            layout->addWidget(reviewButton);
            layout->setAlignment(Qt::AlignCenter);
            cellWidget->setLayout(layout);
            table->setCellWidget(i, 10, cellWidget);
            
            connect(reviewButton, &QPushButton::clicked, this, [=]() {
                // 创建输入对话框
                QDialog dialog(this);
                dialog.setWindowTitle("课程评价");
                
                QFormLayout form(&dialog);
                
                // 添加内容输入框
                QPlainTextEdit *contentEdit = new QPlainTextEdit(&dialog);
                form.addRow("评论内容:", contentEdit);
                
                // 添加优先级选择框
                QSpinBox *prioritySpin = new QSpinBox(&dialog);
                prioritySpin->setRange(1, 5);
                form.addRow("优先级(1-5):", prioritySpin);

                QSpinBox *lisSpin=new QSpinBox(&dialog);
                lisSpin->setRange(1,5);
                form.addRow("听感好坏(1-5)",lisSpin);

                QSpinBox *hwSpin=new QSpinBox(&dialog);
                hwSpin->setRange(1,5);
                form.addRow("作业量大小(1-5)",hwSpin);

                QSpinBox *scSpin=new QSpinBox(&dialog);
                scSpin->setRange(1,5);
                form.addRow("给分好坏(1-5)",scSpin);
                
                // 添加确定按钮
                QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
                form.addRow(&buttonBox);
                
                // 连接按钮信号
                QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
                QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
                
                if (dialog.exec() == QDialog::Accepted) {
                    comment comment;
                    comment.content = contentEdit->toPlainText();
                    comment.priority = prioritySpin->value();
                    comment.hwPrefer=hwSpin->value();
                    comment.listenPrefer=lisSpin->value();
                    comment.scorePrefer=scSpin->value();
                    comment.critic=userinfo->getUsername();
                    comment.semester=semester;
                    comment.teacher=c.teacherList.toVector();

                    // 查找或创建对应的courseComment
                    bool found = false;
                    for (auto &courseComment : *m_allCourseComments) {
                        if (courseComment.code == c.code) {
                            courseComment.comments.append(comment);
                            found = true;
                            break;
                        }
                    }
                    
                    if (!found) {
                        courseComment newCourseComment;
                        newCourseComment.code = c.code;
                        newCourseComment.comments.append(comment);
                        m_allCourseComments->append(newCourseComment);
                    }
                    // qDebug()<<"Have found\n";                    // 保存评论到JSON文件
                    saveCommentToJson(m_allCourseComments);
                    // qDebug() << "Saved comment for course: " << c.code<<Qt::endl;
                    QMessageBox::information(this, "成功", "课程评价已保存！");
                    emit commentUpdated();
                }
            });
        }/*添加测评*/;
        if (type==2){
            QWidget *cellWidget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(cellWidget);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(4);

            QPushButton *electButton = new QPushButton("选课");
            electButton->setFixedSize(80,24);
            electButton->setStyleSheet("QPushButton { font-size: 12px; padding: 2px; background-color: #f0f0f0;}QPushButton:hover { background-color: #e0e0e0;}");
            
            QPushButton *deleteButton= new QPushButton("移除");
            deleteButton->setFixedSize(80,24);
            deleteButton->setStyleSheet("QPushButton { font-size: 12px; padding: 2px; background-color: #f0f0f0;}QPushButton:hover { background-color: #e0e0e0;}");

            connect(deleteButton,&QPushButton::clicked,this,[=](){
                auto &favorites=userinfo->getFavorites();
                favorites.erase(std::remove_if(favorites.begin(),favorites.end(),[&](const CourseInfo&cc){
                    return cc.code==c.code;
                }));
                QMessageBox::information(this,"Tips","请重新进入收藏夹查看");
                //emit favoritesUpdated();
            });

            connect(electButton,&QPushButton::clicked,this,[this,userinfo,c](){
                if (userinfo->getCurrentCourses().contains(c)){
                    QMessageBox::information(this,"Tips","请勿重复添加课程");
                    return;
                }
                int votees=QInputDialog::getInt(this,"海淀赌场","投点数",0,0,userinfo->getRemainingPoints());
                userinfo->getCurrentCourses().push_back(c);
                userinfo->setPointForCourse(c.code,votees);
                QMessageBox::information(this,"Tips","选课成功");

            });



            layout->addWidget(electButton);
            layout->addWidget(deleteButton);
            layout->setAlignment(Qt::AlignCenter);
            cellWidget->setLayout(layout);
            table->setCellWidget(i, 11, cellWidget);
            
        }
        if (type == 0){
            TeacherInfo *teacher = new TeacherInfo(this);
            if (teacher->GetHasDoneLottery() == true){
                bool Lottery_Result = userinfo->getResultForCourse(c.code);
                // QLabel *Result = new QLabel(Lottery_Result?"已选上":"未选上",this);
                // if (!Lottery_Result)
                //     Result->setStyleSheet("color: red;");
                setCenteredItem(11,Lottery_Result?"已选上":"未选上");
                continue;
            }
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
            QSvgRenderer renderer(QString(":/resources/icon/warning.svg"));
            QPixmap pixmap(16, 16);
            pixmap.fill(Qt::transparent);

            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);

            renderer.render(&painter, QRectF(0, 0, 16, 16));

            infoIcon->setPixmap(pixmap);
            // infoIcon->setScaledContents(true);
            // infoIcon->setPixmap(QPixmap(":/resources/icon/warning1.svg").scaled(16, 16));
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
