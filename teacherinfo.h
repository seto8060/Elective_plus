#pragma once
#include <QObject>
#include <QVector>
#include <QSet>
#include <QRandomGenerator>
#include "term.h"
class TeacherInfo : public QObject {
    Q_OBJECT

public:
    explicit TeacherInfo(QObject *parent = nullptr);

    Term getCurrentTerm() const{
        return currentTerm;
    }
    Term getEnrollmentTerm() const{
        return enrollmentTerm;
    }
    Term getUpcomingTerm() const{
        return upcomingTerm;
    }
    Term getRecentlyEndedTerm() const{
        return lastEnrollmentEnded;
    }
    int getverifycode() const{
        return verifycode;
    }

    void TermPass(){
        currentTerm = currentTerm.nextTerm();
    }
    void ElectiveTermPass(){
        lastEnrollmentEnded = enrollmentTerm;
        upcomingTerm = enrollmentTerm.nextTerm();
        enrollmentTerm.semester = 0;
        return ;
    }
    void ElectiveTermCome(){
        enrollmentTerm = upcomingTerm;
        lastEnrollmentEnded.semester = upcomingTerm.semester = 0;
        HasDoneLottery = false;
        return ;
    }

    Term canModifyCourse() const{
        if (upcomingTerm.semester != 0) return upcomingTerm;
        return Term(-1,-1);
    }
    Term canDoLottery() const{
        if (lastEnrollmentEnded.semester != 0) return lastEnrollmentEnded;
        return Term(-1,-1);
    }
    Term canExportStudentInfo() const{
        if (lastEnrollmentEnded.semester != 0 && HasDoneLottery == true) return lastEnrollmentEnded;
        return Term(-1,-1);
    }
    Term canManuallyElect() const{
        if (enrollmentTerm.semester != 0) return enrollmentTerm;
        return Term(-1,-1);
    }
    void save();
    void refreshVerifyCode(){
        verifycode = QRandomGenerator::global()->bounded(1000, 10000);
    }

private:
    //作为教务维护以下变量：
    Term currentTerm = Term(2024,2);            // 当前正在进行的学期
    Term enrollmentTerm = Term(2025,1);         // 当前正在选课的学期
    Term upcomingTerm = Term(2025,0);           // 即将开始选课的学期
    Term lastEnrollmentEnded = Term(2025,0);    // 刚刚结束选课的学期
    bool HasDoneLottery = false;
    int verifycode = 1024;
};
