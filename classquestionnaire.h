#ifndef CLASSQUESTIONNAIRE_H
#define CLASSQUESTIONNAIRE_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>
#include <QFont>
#include <QStyleFactory>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QMap>
#include "numbar.h"
#include "coursecomment.h"
#include "coursesearchpage.h"

class ClassQuestionnaire : public QMainWindow
{
    Q_OBJECT
public:
    ClassQuestionnaire(UserInfo *USER, QVector<CourseInfo> AllCourses, QVector<courseComment> Allcomments, QWidget *parent = nullptr);
private slots:
    void startCollection();
    void nextPage();
    void prevPage();
    void endPage();
    void resultPage();
    void restartCollection();
    void finishPage();
    void returnPage();
signals:
    void coursesUpdated();
    void favoritesUpdated();
private:
    UserInfo *user;
    void createWelcomeWidget();
    void createScoreCollectionWidget();
    void createNeedCollectionWidget();
    void createSummaryWidget();
    void createResultWidget();
    void createFinishWidget();
    void setRatingDisplay(QTableWidgetItem *item, double total, double hw, double exam, double listen, int cnt);
    void handleElectCourse(const CourseInfo& course);
    bool isHandlingEnroll=false;
    void collectResult();
    QString getSummaryText();
    double square(double x);
    bool checkEight(QString Time);
    double lossCalculation(CourseInfo course);
    bool checkTimeString(QString a, QString b);
    bool checkTime(CourseInfo course);
    QVBoxLayout *mainLayout;
    QStackedWidget *stackedWidget;
    QWidget *welcomeWidget;
    QWidget *scoreWidget;
    QWidget *needWidget;
    QWidget *summaryWidget;
    QWidget *resultWidget;
    QWidget *finishWidget;
    QWidget *returnWidget;

    QPushButton *startButton;
    QPushButton *nextPageButton;
    QPushButton *prevPageButton;
    QPushButton *submitButton;
    QPushButton *restartButton;
    QPushButton *resultButton;
    QPushButton *restartButton_2;
    QPushButton *workButton;
    QPushButton *returnButton;

    int currentIndex;
    QLabel *scoreTitle;
    NumBar *scoreMin;
    NumBar *scoreMax;
    NumBar *classMin;
    NumBar *classMax;
    NumBar *EnglishNeed;
    NumBar *PENeed;
    NumBar *publicNeed;
    NumBar *generalNeed;
    QLabel *needTitle;
    NumBar *gradeNeed;
    NumBar *experienceNeed;
    NumBar *loadNeed;
    NumBar *eightNeed;
    NumBar *freedayNeed;
    NumBar *luckNeed;
    QLabel *summaryTitle;
    QLabel *SummaryText;
    QLabel *resultTitle;
    QLabel *resultSummary;
    QTableWidget *courseTable;
    struct CourseWithLoss{
        CourseWithLoss(CourseInfo course, double loss):
            course(course), loss(loss){}
        CourseInfo course;
        double loss;

        bool operator < (const CourseWithLoss &tmp){
            if(loss == tmp.loss)
                return course.code < tmp.course.code;
            return loss < tmp.loss;
        }

    };
    QVector<CourseWithLoss> courses;
    QVector<CourseInfo> AnswerCourses;
    QVector<CourseInfo> all_courses;
    QVector<courseComment> all_comments;
    QMap<QString, int> quesResult;



    bool timeCheck(CourseInfo course, QVector<CourseInfo> classList){
        for(int i = 0; i < classList.size(); ++ i){
            QStringList tmp = classList[i].timeList;
            QStringList _tmp = course.timeList;
            for(int j = 0; j < tmp.size(); ++j){
                for(int k = 0; k < _tmp.size(); ++k){
                    if(!checkTimeString(tmp[j], _tmp[k]))
                        return 0;
                }
            }
        }
        return 1;
    }

    QVector<CourseInfo> selectCourseNow;
    void selectOneCourse(const CourseInfo& course) {
        user->getCurrentCourses().push_back(course);
        user->setPointForCourse(course.code, 0);
        emit coursesUpdated();
    }

};

#endif // CLASSQUESTIONNAIRE_H
