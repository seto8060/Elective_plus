#include "classquestionnaire.h"
#include <algorithm>
#include <QHeaderView>
#include <QInputDialog>
ClassQuestionnaire::ClassQuestionnaire(UserInfo *USER, QVector<CourseInfo> AllCourses, QVector<courseComment> AllComments,QWidget *parent) :
    QMainWindow(parent),
    user(USER),
    all_courses(AllCourses),
    all_comments(AllComments){

    QFont appFont("Segoe UI", 10);
    QApplication::setFont(appFont);

    QWidget *centralWidget = new QWidget(this);
    centralWidget -> setStyleSheet("background-color: #f5f7fa;");
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("background-color: white; border-radius: 15px;");
    mainLayout->addWidget(stackedWidget, 1);

    createWelcomeWidget();
    createScoreCollectionWidget();
    createNeedCollectionWidget();
    createSummaryWidget();
    createResultWidget();

    connect(startButton, &QPushButton::clicked, this, &ClassQuestionnaire::startCollection);
    connect(nextPageButton, &QPushButton::clicked, this, &ClassQuestionnaire::nextPage);
    connect(prevPageButton, &QPushButton::clicked, this, &ClassQuestionnaire::prevPage);
    connect(submitButton, &QPushButton::clicked, this, &ClassQuestionnaire::endPage);
    connect(restartButton, &QPushButton::clicked, this, &ClassQuestionnaire::restartCollection);
    connect(restartButton_2, &QPushButton::clicked, this, &ClassQuestionnaire::restartCollection);
    connect(resultButton, &QPushButton::clicked, this, &ClassQuestionnaire::resultPage);
    connect(workButton, &QPushButton::clicked, this, &ClassQuestionnaire::finishPage);

    stackedWidget -> setCurrentIndex(0);
}

void ClassQuestionnaire::startCollection(){
    currentIndex = 0;
    quesResult.clear();

    stackedWidget -> setCurrentIndex(1);
}

void ClassQuestionnaire::nextPage(){
    currentIndex ++;
    stackedWidget -> setCurrentIndex(currentIndex + 1);
}

void ClassQuestionnaire::prevPage(){
    currentIndex --;
    stackedWidget -> setCurrentIndex(currentIndex + 1);
}

void ClassQuestionnaire::endPage(){
    collectResult();
    SummaryText -> setText(getSummaryText());
    currentIndex ++;
    stackedWidget -> setCurrentIndex(currentIndex + 1);
}

void ClassQuestionnaire::setRatingDisplay(QTableWidgetItem *item, double total, double hw, double exam, double listen, int cnt)
{
    QString emoji;
    if (cnt == 0) {
        emoji = "😶‍🌫️";
    } else {
        if (total >= 4) emoji = "😍";
        else if (total >= 2.0) emoji = "😑";
        else emoji = "😞";
    }

    item->setText(emoji);
    item->setTextAlignment(Qt::AlignCenter);

    QString toolTip = QString("评价数：%1\n总评: %2\n听感: %3\n作业量: %4\n给分: %5")
                          .arg(cnt)
                          .arg(total)
                          .arg(listen)
                          .arg(hw)
                          .arg(exam);
    item->setToolTip(toolTip);
}

void ClassQuestionnaire::handleElectCourse(const CourseInfo& course) {
    if (isHandlingEnroll) return;
    isHandlingEnroll = true;

    bool ok;
    int votees = QInputDialog::getInt(this, "海淀赌场", "投点数：", 0, 0, 100, 1, &ok);
    if (!ok) {
        isHandlingEnroll = false;
        return;
    }

    QSet<QPair<QString, QString>> existingCourses;
    for (const auto& enrolled : user->getCurrentCourses()) {
        for (const auto& time : enrolled.timeList) {
            for (const auto& w : enrolled.week) {
                existingCourses.insert({time, w});
            }
        }
    }

    bool hasConflict = false;
    if (votees > user->getRemainingPoints()) {
        QMessageBox::information(this, "Tips", "超额投点。");
        isHandlingEnroll = false;
        return;
    }

    for (const auto& time : course.timeList) {
        for (const auto& w : course.week) {
            if (existingCourses.contains({time, w})) {
                hasConflict = true;
                break;
            }
        }
        if (hasConflict) break;
    }

    if (hasConflict) {
        QMessageBox::information(this, "Tips", "选课失败，与已选课程时间冲突！");
    } else {
        user->getCurrentCourses().push_back(course);
        user->setPointForCourse(course.code, votees);
        QMessageBox::information(this, "Tips", "选课成功！");
        emit coursesUpdated();
    }

    isHandlingEnroll = false;
}


void ClassQuestionnaire::resultPage(){
    int classNum = quesResult["PE"] + quesResult["Eng"] + quesResult["public"] + quesResult["general"];
    if (classNum > quesResult["classMax"]){
        QMessageBox::information(this, "数据错误", "所需课程数有误！请重新输入");
        return ;
    }
    if (quesResult["scoreMax"] < quesResult["scoreMin"]){
        QMessageBox::information(this, "数据错误", "最多学分不得低于最少学分！请重新输入");
        return ;
    }
    if (quesResult["classMax"] < quesResult["classMin"]){
        QMessageBox::information(this, "数据错误", "最多课程数不得低于少低课程数！请重新输入");
        return ;
    }

    courses.clear();
    for (int i = 0; i < all_courses.size(); i ++){
        if(all_courses[i].type == "专业必修" || all_courses[i].type == "专业任选" || all_courses[i].type == "专业限选"){
            continue;
        }
        if(all_courses[i].type == "军事理论" || all_courses[i].type == "思想政治" || all_courses[i].type == "劳动教育课程"){
            continue;
        }
        if(checkTime(all_courses[i])){
            double loss = lossCalculation(all_courses[i]);
            courses.append(CourseWithLoss(all_courses[i], loss));
        }
    }
    std::sort(courses.begin(), courses.end());


    QVector<CourseWithLoss> PE;
    QVector<CourseWithLoss> Eng;
    QVector<CourseWithLoss> General;
    QVector<CourseWithLoss> Public;

    //1 score
    for (int i = 0; i < courses.size(); i ++){
        if(courses[i].course.score.toInt() == 0) continue;
        if(courses[i].course.index.toInt() >= 2164 && courses[i].course.index.toInt() <= 2169) continue;
        if(courses[i].course.type == "体育" && courses[i].course.name != "太极拳" && courses[i].course.name != "健美操")
            PE.append(courses[i]);
    }

    //2 score
    for (int i = 0; i < courses.size(); i ++){
        if(courses[i].course.score.toInt() == 0) continue;
        if(courses[i].course.type == "大学英语" && (i == 0 || courses[i].course.index != courses[i - 1].course.index))
            Eng.append(courses[i]);
    }

    // 2/ 3/ 4
    for (int i = 0; i < courses.size(); i ++){
        if(courses[i].course.score.toInt() == 0) continue;
        if(courses[i].course.type == "通选课" && (i == 0 || courses[i].course.index != courses[i - 1].course.index))
            General.append(courses[i]);
    }

    //1/ 2/ 3/ 4
    for (int i = 0; i < courses.size(); i ++){
        if(courses[i].course.score.toInt() == 0) continue;
        if(courses[i].course.type == "全校公选课" && (i == 0 || courses[i].course.index != courses[i - 1].course.index))
            Public.append(courses[i]);
    }

    if(quesResult["PE"]){
        if(PE.size() == 0){
            QMessageBox::information(this, "选课中止", "没有符合您条件的体育课！");
            return ;
        }
    }

    if(quesResult["Eng"]){
        if(Eng.size() == 0){
            QMessageBox::information(this, "选课中止", "没有符合您条件的英语课！");
            return ;
        }
    }

    if(quesResult["general"]){
        if(General.size() == 0){
            QMessageBox::information(this, "选课中止", "没有符合您条件的通识课！");
            return ;
        }
    }

    if(quesResult["public"]){
        if(Public.size() == 0){
            QMessageBox::information(this, "选课中止", "没有符合您条件的公选课!");
            return ;
        }
    }

    struct PlanLoss{
        double preLoss;
        QVector<CourseInfo> selectClasses;
        bool vis[7];
        int freedays;
        PlanLoss(){}
        PlanLoss(UserInfo *user){
            preLoss = 2000.0;
            freedays = 0;
            for (int i = 0; i < 7; ++ i) vis[i] = 0;
            const QVector<CourseInfo> curCourses = user -> getCurrentCourses();
            const QString days[7] = {"一", "二", "三", "四", "五", "六", "日"};
            for (int i = 0; i < curCourses.size(); ++ i){
                CourseInfo tmpCourse = curCourses[i];

                for (int j = 0; j < tmpCourse.timeList.size(); j ++){
                    QString tmp = tmpCourse.timeList[j];

                    for (int k = 0; k < 7; ++ k)
                        if(tmp.contains(days[k])){
                            vis[k] = 1;
                            break;
                        }

                }
            }

            for (int i = 0; i < 7; ++ i)
                if(vis[i] == 0)
                    freedays ++;
            selectClasses.clear();
        }

        int ifChangeFree(CourseInfo course){
            const QString days[7] = {"一", "二", "三", "四", "五", "六", "日"};
            int ans = 0;
            for (int i = 0; i < course.timeList.size(); ++ i){
                QString tmp = course.timeList[i];

                for (int j = 0; j < 7; ++ j)
                    if(tmp.contains(days[j])){
                        if(!vis[j]) ans ++;
                    }
            }
            return ans;
        }
        void changeFree(CourseInfo course){
            const QString days[7] = {"一", "二", "三", "四", "五", "六", "日"};
            for (int i = 0; i < course.timeList.size(); ++ i){
                QString tmp = course.timeList[i];

                for (int j = 0; j < 7; ++ j)
                    if(tmp.contains(days[j])){
                        vis[j] = 1;
                        freedays --;
                    }
            }
        }
        void addCourse(CourseWithLoss course){
            selectClasses.append(course.course);
            if(ifChangeFree(course.course))
                changeFree(course.course);
            preLoss += course.loss;
        }

    };

    //2000 * 10 * 10
    PlanLoss dp[25][10][10];

    for (int i = 0; i < 25; i ++)
        for (int l = 0; l < 10; l ++)
            for (int m = 0; m < 10; m ++)
                dp[i][l][m] = PlanLoss(user);

    dp[0][0][0].preLoss = 0;

    int numPE = quesResult["PE"];
    int numEng = quesResult["Eng"];
    int numGen = quesResult["general"];
    int numPub = quesResult["public"];

    if(numPE){
        for (int i = 0; i < PE.size(); ++ i){
            if(dp[0][0][0].preLoss + quesResult["freeday"] * dp[0][0][0].ifChangeFree(PE[i].course) + PE[i].loss < dp[1][0][0].preLoss){
                PlanLoss tmp = dp[0][0][0];
                tmp.addCourse(PE[i]);
                dp[1][0][0] = tmp;
            }
        }
    }

    if(numEng){
        for (int i = 0; i < Eng.size(); ++ i){
            if(dp[numPE][0][0].preLoss + quesResult["freeday"] * dp[numPE][0][0].ifChangeFree(Eng[i].course) + Eng[i].loss < dp[numPE + 2][0][0].preLoss){
                PlanLoss tmp = dp[numPE][0][0];
                tmp.addCourse(Eng[i]);
                dp[numPE + 2][0][0] = tmp;
            }
        }
    }

    for (int i = 0; i < General.size(); ++ i){
        for (int j = 24; j >= General[i].course.score.toInt() + numPE + 2 * numEng; j --){
            for (int k = 9; k >= 1; k --){
                if(dp[j - General[i].course.score.toInt()][k - 1][0].preLoss + quesResult["freeday"] * dp[j - General[i].course.score.toInt()][k - 1][0].ifChangeFree(General[i].course) + General[i].loss < dp[j][k][0].preLoss){
                    PlanLoss tmp = dp[j - General[i].course.score.toInt()][k - 1][0];
                    tmp.addCourse(General[i]);
                    dp[j][k][0] = tmp;
                }
            }
        }
    }

    for (int num = 9; num >= 0; num --)
        for (int i = 0; i < Public.size(); ++ i){
            for (int j = 24; j >= Public[i].course.score.toInt() + numPE + 2 * numEng; j --){
                for (int k = 9; k >= 1; k --){
                    if(dp[j - Public[i].course.score.toInt()][num][k - 1].preLoss + quesResult["freeday"] * dp[j - Public[i].course.score.toInt()][num][k - 1].ifChangeFree(Public[i].course) + Public[i].loss < dp[j][num][k - 1].preLoss){
                        PlanLoss tmp = dp[j - Public[i].course.score.toInt()][num][k - 1];
                        tmp.addCourse(Public[i]);
                        dp[j][num][k] = tmp;
                    }
                }
            }
        }

    double MinLoss[26];
    for (int i = 0; i <= 25; ++ i)
        MinLoss[i] = 2000.0;

    bool flag = 0;
    QVector<CourseInfo> ans[26];
    for (int i = numGen; i <= 9; i ++){
        for (int j = numPub; j <= 9; j ++){
            if (i + j + numPE + numEng < quesResult["classMin"]) continue;
            if (i + j + numPE + numEng > quesResult["classMax"]) break;
            for (int k = quesResult["scoreMin"]; k <= quesResult["scoreMax"]; k ++){
                if(dp[k][i][j].preLoss >= 1000) continue;
                if(dp[k][i][j].preLoss < MinLoss[k]){
                    MinLoss[k] = dp[k][i][j].preLoss;
                    ans[k] = dp[k][i][j].selectClasses;
                    flag = 1;
                }
            }
        }
    }


    if(!flag){
        QMessageBox::information(this, "选课失败", "当前无法满足您的选课需求");
        return ;
    }

    int Maxscore = 0, class_Num = 0;
    for (int i = quesResult["scoreMax"]; i >= quesResult["scoreMin"]; i --){
        if(MinLoss[i] != 2000.0){
            Maxscore = i;
            class_Num = ans[i].size();
        }
    }
    resultSummary -> setText(QString("智能选课系统根据需求选出了%1门课，共%2学分\n").arg(class_Num).arg(Maxscore));

    courseTable->setRowCount(ans[Maxscore].size());
    for (int i = 0; i < ans[Maxscore].size(); ++i) {
        const CourseInfo &c = ans[Maxscore][i];
        courseTable->setItem(i, 0, new QTableWidgetItem(c.code));
        courseTable->setItem(i, 1, new QTableWidgetItem(c.name));
        courseTable->setItem(i, 2, new QTableWidgetItem(c.type));
        courseTable->setItem(i, 3, new QTableWidgetItem(c.teacherList.join(", ")));
        courseTable->setItem(i, 4, new QTableWidgetItem(c.timeList.join("；")));
        courseTable->setItem(i, 5, new QTableWidgetItem(c.unit));

        // ⭐ 评分逻辑
        courseComment* tar = nullptr;
        for (auto& v : all_comments) {
            if (v.code == c.code) {
                tar = &v;
                break;
            }
        }
        if (!tar) {
            all_comments.push_back(courseComment{c.code});
            tar = &all_comments.last();
        }

        QVector<comment> thisCourseComments;
        for (const auto& v : tar->comments) {
            if (v.teacher.join("；") == c.teacherList.join("")) {
                thisCourseComments.append(v);
            }
        }

        int cnt = thisCourseComments.size();
        double total = 0, listen = 0, exam = 0, hw = 0;
        for (const auto& v : thisCourseComments) {
            total += v.priority;
            listen += v.listenPrefer;
            exam += v.scorePrefer;
            hw += v.hwPrefer;
        }
        if (cnt > 0) {
            total /= cnt;
            listen /= cnt;
            exam /= cnt;
            hw /= cnt;
        }


        QTableWidgetItem* ratingItem = new QTableWidgetItem();
        setRatingDisplay(ratingItem, total, hw, exam, listen, cnt);
        courseTable->setItem(i, 7, ratingItem);

        // ⭐ 操作按钮
        QPushButton* detailButton = new QPushButton("查看详情", this);
        QPushButton* addToFavorButton = new QPushButton("添加到收藏夹", this);
        QPushButton* electCourseButton = new QPushButton("选课", this);

        connect(detailButton, &QPushButton::clicked, [=]() {
            CourseDetailPage *detailPage = new CourseDetailPage(c, tar,this);
            connect(detailPage, &CourseDetailPage::backRequested, [this, detailPage]() {
                stackedWidget->removeWidget(detailPage);
                detailPage->deleteLater();
            });
            connect(detailPage, &CourseDetailPage::enrollRequested, this, [this, c](const QString& code) {
                if (code == c.code) {
                    handleElectCourse(c);
                } else {
                    QMessageBox::warning(this, "错误", "课程编号不匹配，请刷新页面后重试。");
                }
            });
            stackedWidget->addWidget(detailPage);
            stackedWidget->setCurrentWidget(detailPage);
        });

        connect(addToFavorButton, &QPushButton::clicked, [=]() {
            if (!user->getFavorites().contains(c)) {
                user->getFavorites().push_back(c);
                QMessageBox::information(this, "Tips", "Success!");
            } else {
                QMessageBox::information(this, "Tips", "请勿重复添加课程！");
            }
        });

        connect(electCourseButton, &QPushButton::clicked, [=]() {
            handleElectCourse(c);
        });

        QWidget* buttonContainer = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(2, 2, 2, 2);
        buttonLayout->setSpacing(5);
        buttonLayout->addWidget(detailButton);
        buttonLayout->addWidget(addToFavorButton);
        buttonLayout->addWidget(electCourseButton);

        courseTable->setCellWidget(i, 6, buttonContainer);  // 假设第7列是操作列
    }
    currentIndex ++;
    stackedWidget -> setCurrentIndex(currentIndex + 1);
}

void ClassQuestionnaire::finishPage(){
    currentIndex = 0;
    quesResult.clear();
    stackedWidget -> setCurrentIndex(0);
}

void ClassQuestionnaire::restartCollection(){
    stackedWidget -> setCurrentIndex(0);
}

bool ClassQuestionnaire::checkTimeString(QString a, QString b){
    const QString days[7] = {"一", "二", "三", "四", "五", "六", "日"};

    for (int i = 0; i < 7; i ++){
        if(a.contains(days[i])){
            if(!b.contains(days[i]))
                return 1;
        }
    }

    QRegularExpression re("(\\d+)");
    QRegularExpressionMatchIterator aNum = re.globalMatch(a);
    QRegularExpressionMatchIterator bNum = re.globalMatch(b);

    int aL = 0, aR = 0, bL = 0, bR = 0;
    QRegularExpressionMatch match;
    if(aNum.hasNext()){
        match = aNum.next();
        aL = match.captured(0).toInt();
    }

    if(aNum.hasNext()){
        match = aNum.next();
        aR = match.captured(0).toInt();
    }

    if(bNum.hasNext()){
        match = bNum.next();
        bL = match.captured(0).toInt();
    }

    if(bNum.hasNext()){
        match = bNum.next();
        bR = match.captured(0).toInt();
    }

    if(aR <= bL || bR <= aL) return 1;
    return 0;
}

bool ClassQuestionnaire::checkTime(CourseInfo course){
    const QVector<CourseInfo> curCourses = user -> getCurrentCourses();

    for (int i = 0; i < curCourses.size(); i ++){
        CourseInfo tmpCourse = curCourses[i];

        for (int j = 0; j < tmpCourse.timeList.size(); j ++){
            for (int k = 0; k < course.timeList.size(); k ++){
                if(!checkTimeString(tmpCourse.timeList[j], course.timeList[k])){
                    return 0;
                }
            }
        }
    }
    return 1;
}

void ClassQuestionnaire::collectResult(){
    quesResult["scoreMin"] = scoreMin -> value();
    quesResult["scoreMax"] = scoreMax -> value();
    quesResult["classMin"] = classMin -> value();
    quesResult["classMax"] = classMax -> value();

    quesResult["Eng"] = EnglishNeed -> value();
    quesResult["PE"] = PENeed -> value();
    quesResult["public"] = publicNeed -> value();
    quesResult["general"] = generalNeed -> value();

    quesResult["eight"] = eightNeed -> value();
    quesResult["load"] = loadNeed -> value();
    quesResult["experience"] = experienceNeed -> value();
    quesResult["grade"] = gradeNeed -> value();
    quesResult["freeday"] = freedayNeed -> value();
    quesResult["luck"] = luckNeed -> value();
}

QString ClassQuestionnaire::getSummaryText(){
    QString summaryText;
    summaryText += QString("您所需的学分数最少为%1，最多为%2\n")
                       .arg(quesResult["scoreMin"])
                       .arg(quesResult["scoreMax"]);
    summaryText += QString("您所需选的课程数最少为%1，最多为%2\n")
                       .arg(quesResult["classMin"])
                       .arg(quesResult["classMax"]);
    if(quesResult["PE"]) summaryText += "您需要选1门体育课\n";

    if(quesResult["Eng"]) summaryText += "您需要选1门英语课\n";

    if(quesResult["general"]) summaryText += QString("您需要选至少%1门通识课\n")
                           .arg(quesResult["general"]);
    if(quesResult["public"]) summaryText += QString("您需要选至少%1门公选课\n")
                           .arg(quesResult["public"]);
    return summaryText;
}

//严重的更改文案需求
void ClassQuestionnaire::createWelcomeWidget(){
    welcomeWidget = new QWidget;

    welcomeWidget -> setStyleSheet("background: #CCCCCC");

    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeWidget);
    welcomeLayout -> setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("智能选课系统");
    titleLabel -> setStyleSheet("font-size : 48px; font-weight: bold; color: #303D3D");
    titleLabel -> setAlignment(Qt::AlignCenter);

    //系统描述 需要更改文案 更改文字格式
    QString courseSystemDesc =
        "欢迎来到智能选课系统！<br>"
        "为收集您的选课需求，该系统将选课需求划分成<br>"
        "<b>“给分好”</b>"
        "<b>“听感好”</b>"
        "<b>“任务量小”</b>"
        "<b>“不希望上早八”</b>"
        "<b>“希望无课的天数尽可能多”</b>"
        "<b>“需要投点的课尽可能少”</b><br>"
        "六个方面.<br>"
        "请根据您的意愿输入您需要的学分数、课程数，以及您对于各个意愿的权重<br>"
        "该系统会生成若干套尽可能符合需求的选课计划供您选择";
    QLabel *describeLabel = new QLabel(courseSystemDesc);
    describeLabel->setStyleSheet("font-size: 24px; color: rgba(0, 0, 0, 0.85); line-height: 1.6; margin: 30px 0;");
    describeLabel->setAlignment(Qt::AlignCenter);
    //系统描述终止

    startButton = new QPushButton("开始智能选课");
    startButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255,255,255,0.9);"
        "  color: #2c5282;"
        "  border-radius: 25px;"
        "  padding: 15px 40px;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e2e8f0;"
        "}"
        );

    welcomeLayout -> addWidget(titleLabel);
    welcomeLayout -> addWidget(describeLabel);
    welcomeLayout -> addWidget(startButton, 0, Qt::AlignCenter);

    stackedWidget -> addWidget(welcomeWidget);
}

//针对具体课程类型进行处理
//通识核心/1234/英语分级
void ClassQuestionnaire::createScoreCollectionWidget(){
    scoreWidget = new QWidget;
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreWidget);

    scoreLayout -> setContentsMargins(40, 30, 40, 30);
    scoreLayout -> setSpacing(20);

    /*
     考虑的几个问题：
     学分min / max -> numbar
     课程min / max -> numbar
     体育/英语(Y A B C C+) -> 是否的选择形式
     /通识（1234/核心）/公选 -> numbar

     针对每一个具体问题单独处理
     */
    scoreTitle = new QLabel("请根据自己的需求，填入需要的课程学分和课程数目。\n"
                            "对于英语课和体育课，0 = 需要， 1 = 不需要");
    scoreTitle -> setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3748; text-align: center;");
    scoreLayout -> addWidget(scoreTitle);

    scoreMin = new NumBar("1.最少选课学分：", 1, 25, this);
    scoreMax = new NumBar("2.最多选课学分：", 1, 25, this);
    classMin = new NumBar("3.最少选课数量：", 1, 9, this);
    classMax = new NumBar("4.最多选课数量：", 1, 9, this);
    EnglishNeed = new NumBar("5.是否需要英语课：", 0, 1, this);
    PENeed = new NumBar("6.是否需要体育课：", 0, 1, this);
    generalNeed = new NumBar("7.至少需要几节通识课：", 0, 9, this);
    publicNeed = new NumBar("8.至少需要几节公选课：", 0, 9, this);

    scoreLayout -> addWidget(scoreMin);
    scoreLayout -> addWidget(scoreMax);
    scoreLayout -> addWidget(classMin);
    scoreLayout -> addWidget(classMax);
    scoreLayout -> addWidget(EnglishNeed);
    scoreLayout -> addWidget(PENeed);
    scoreLayout -> addWidget(generalNeed);
    scoreLayout -> addWidget(publicNeed);


    QWidget *ButtonWidget = new QWidget;
    QHBoxLayout *ButtonLayout = new QHBoxLayout(ButtonWidget);

    nextPageButton = new QPushButton("下一页");
    nextPageButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4299e1;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 25px;"
        "  font-size: 16px;"
        "  min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3182ce;"
        "}"
        );

    ButtonLayout -> addStretch();
    ButtonLayout -> addWidget(nextPageButton);

    scoreLayout -> addWidget(ButtonWidget);

    stackedWidget -> addWidget(scoreWidget);
}

//6个需求 每个需求分为5个程度 非常需要/比较需要/一般需要/不太需要/不考虑
void ClassQuestionnaire::createNeedCollectionWidget(){
    needWidget = new QWidget;
    QVBoxLayout *needLayout = new QVBoxLayout(needWidget);
    needLayout -> setContentsMargins(40, 30, 40, 30);
    needLayout -> setSpacing(20);

    //title
    needTitle = new QLabel("请根据自己的需求，针对以下6个指标使用0-5进行评估：\n"
                           "0 = 完全不需要\n"
                           "1 = 基本不需要\n"
                           "2 = 不太需要\n"
                           "3 = 一般需要\n"
                           "4 = 比较需要\n"
                           "5 = 非常需要");
    needTitle -> setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3748; text-align: center;");
    needLayout->addWidget(needTitle);

    /*
     选择六个维度的五个需求（暂选择拉条）
     */
    gradeNeed = new NumBar("1.给分好：", 0, 5, this);
    experienceNeed = new NumBar("2.听感好：", 0, 5, this);
    loadNeed = new NumBar("3.任务量小：", 0, 5, this);
    eightNeed = new NumBar("4.不上早八：", 0, 5, this);
    freedayNeed = new NumBar("5.无课天数多：", 0, 5, this);
    luckNeed = new NumBar("6.投点课少：", 0, 5, this);

    needLayout -> addWidget(gradeNeed);
    needLayout -> addWidget(experienceNeed);
    needLayout -> addWidget(loadNeed);
    needLayout -> addWidget(eightNeed);
    needLayout -> addWidget(freedayNeed);
    needLayout -> addWidget(luckNeed);


    QWidget *ButtonWidget = new QWidget;
    QHBoxLayout *ButtonLayout = new QHBoxLayout(ButtonWidget);
    ButtonLayout -> setContentsMargins(0, 0, 0, 0);

    prevPageButton = new QPushButton("上一页");
    prevPageButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #e2e8f0;"
        "  color: #4a5568;"
        "  border-radius: 8px;"
        "  padding: 12px 25px;"
        "  font-size: 16px;"
        "  min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #cbd5e0;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #edf2f7;"
        "  color: #a0aec0;"
        "}"
        );

    submitButton = new QPushButton("完成填写");
    submitButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4299e1;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 25px;"
        "  font-size: 16px;"
        "  min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3182ce;"
        "}"
        );

    ButtonLayout -> addWidget(prevPageButton);
    ButtonLayout -> addStretch();
    ButtonLayout -> addWidget(submitButton);

    needLayout -> addWidget(ButtonWidget);

    stackedWidget -> addWidget(needWidget);
}

void ClassQuestionnaire::createSummaryWidget(){

    summaryWidget = new QWidget;
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryWidget);
    summaryLayout->setContentsMargins(40, 30, 40, 30);
    summaryLayout->setSpacing(20);

    summaryTitle = new QLabel("调查完成！");
    summaryTitle -> setStyleSheet("font-size: 40px; font-weight: bold; color: #2d3748; text-align: center;");
    summaryTitle -> setAlignment(Qt::AlignCenter);
    summaryLayout -> addWidget(summaryTitle);

    SummaryText = new QLabel;
    SummaryText -> setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3748; text-align: center;");
    SummaryText -> setAlignment(Qt::AlignCenter);
    summaryLayout -> addWidget(SummaryText);

    QWidget *ButtonWidget = new QWidget;
    QHBoxLayout *ButtonLayout = new QHBoxLayout(ButtonWidget);
    ButtonLayout->setContentsMargins(0, 0, 0, 0);

    restartButton = new QPushButton("重新填写");
    restartButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #48bb78;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 30px;"
        "  font-size: 16px;"
        "  min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #38a169;"
        "}"
        );

    resultButton = new QPushButton("查看智能选课结果");
    resultButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4299e1;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 30px;"
        "  font-size: 16px;"
        "  min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3182ce;"
        "}"
        );

    ButtonLayout -> addStretch();
    ButtonLayout -> addWidget(restartButton);
    ButtonLayout -> addSpacing(20);
    ButtonLayout -> addWidget(resultButton);
    ButtonLayout -> addStretch();

    summaryLayout -> addWidget(ButtonWidget);

    stackedWidget -> addWidget(summaryWidget);
}

void ClassQuestionnaire::createResultWidget(){

    resultWidget = new QWidget;
    QVBoxLayout *resultLayout = new QVBoxLayout(resultWidget);
    resultLayout -> setContentsMargins(40, 30, 40, 30);
    resultLayout -> setSpacing(20);



    resultTitle = new QLabel("智能选课结果：");
    resultTitle -> setStyleSheet("font-size : 40px; font-weight: bold; color: #1B2629");
    resultTitle -> setAlignment(Qt::AlignCenter);
    resultLayout -> addWidget(resultTitle);

    resultSummary = new QLabel;
    resultSummary -> setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3748; text-align: center;");
    resultSummary -> setAlignment(Qt::AlignCenter);
    resultLayout -> addWidget(resultSummary);



    courseTable = new QTableWidget(this);
    courseTable->setColumnCount(8); // 编号、名称、教师、时间、单位、操作、marks
    courseTable->setHorizontalHeaderLabels({"课程编号", "课程名称","课程类别", "授课教师", "上课时间", "开课单位", "操作", "评分"});
    courseTable->horizontalHeader()->setStretchLastSection(false);
    courseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    courseTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    courseTable->setColumnWidth(0, 100);
    courseTable->setColumnWidth(1, 130);
    courseTable->setColumnWidth(2, 80);
    courseTable->setColumnWidth(3, 80);
    courseTable->setColumnWidth(4, 330);
    courseTable->setColumnWidth(5, 130);
    courseTable->setColumnWidth(6, 300);
    courseTable->setColumnWidth(7, 80);

    courseTable->setWordWrap(true);
    courseTable->resizeRowsToContents();
    courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    resultLayout -> addWidget(courseTable);

    QWidget *ButtonWidget = new QWidget;
    QHBoxLayout *ButtonLayout = new QHBoxLayout(ButtonWidget);

    workButton = new QPushButton("返回");
    workButton -> setStyleSheet(
        "QPushButton {"
        "  background-color: #48bb78;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 30px;"
        "  font-size: 16px;"
        "  min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #38a169;"
        "}"
        );
    restartButton_2 = new QPushButton("重新填写");
    restartButton_2 -> setStyleSheet(
        "QPushButton {"
        "  background-color: #48bb78;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 12px 30px;"
        "  font-size: 16px;"
        "  min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #38a169;"
        "}"
        );
    ButtonLayout -> addStretch();
    ButtonLayout -> addWidget(workButton);
    ButtonLayout -> addStretch();
    ButtonLayout -> addWidget(restartButton_2);
    ButtonLayout -> addStretch();

    resultLayout -> addWidget(ButtonWidget);


    stackedWidget -> addWidget(resultWidget);
}

double ClassQuestionnaire::square(double x){return x * x;}

bool ClassQuestionnaire::checkEight(QString Time){
    QRegularExpression re("(\\d+)");
    QRegularExpressionMatchIterator Num = re.globalMatch(Time);
    QRegularExpressionMatch match;
    if(Num.hasNext()){
        match = Num.next();
        int num = match.captured(0).toInt();
        if(num == 1) return 1;
        else return 0;
    }
    else return 0;
}

double ClassQuestionnaire::lossCalculation(CourseInfo course){
    double loss = 0;
    QString courseIndex = course.code;
    courseComment coursecomment;

    for (int i = 0; i < all_comments.size(); i ++){
        if(all_comments[i].code == courseIndex){
            coursecomment = all_comments[i];
            break;
        }
    }

    if(coursecomment.comments.size() == 0){
        return 0;
    }

    double priority = 0, load = 0, score = 0, experience = 0;
    for (int i = 0; i < coursecomment.comments.size(); i ++){
        priority += coursecomment.comments[i].priority;
        load += coursecomment.comments[i].hwPrefer;
        score += coursecomment.comments[i].scorePrefer;
        experience += coursecomment.comments[i].listenPrefer;
    }

    load /= coursecomment.comments.size();
    score /= coursecomment.comments.size();
    experience /= coursecomment.comments.size();

    loss += 0.1 * square(5 - load) * (quesResult["load"] + 0.1);
    loss += 0.1 * square(5 - score) * (quesResult["grade"] + 0.1);
    loss += 0.1 * square(5 - experience) * (quesResult["experience"] + 0.1);

    bool eightFlag = 0;
    for (int i = 0; i < course.timeList.size(); ++ i){
        if(checkEight(course.timeList[i]))
            eightFlag = 1;
    }
    loss += (eightFlag ? 3 : 0.0) * quesResult["eight"];

    double personRate = course.Now_person * 1.0 / course.Max_person;

    if(personRate > 1.0){
        loss += personRate * 3 * quesResult["luck"];
    }

    priority /= coursecomment.comments.size();

    loss *= 8.0 / priority;
    return loss;
}
