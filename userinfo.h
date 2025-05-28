#pragma once
#include <QString>
#include <QVector>
#include "CourseInfo.h"
#include <QJsonObject>

class UserInfo {
public:
    UserInfo();
    UserInfo(const QString &username,
             const QString &password,
             const QString &grade,
             const QString &college,
             const QString &Index,
             const QString &Realname,
             const bool &IsTeacher);

    QString getUsername() const;
    QString getPassword() const;
    QString getGrade() const;
    QString getCollege() const;
    QString getIndex() const;
    QString getRealname() const;
    int getPoint() const;

    void setPassword(const QString &pwd);
    void setGrade(const QString &grade);
    void setCollege(const QString &college);

    QVector<CourseInfo>& getCurrentCourses() ;
    QVector<CourseInfo>& getFavorites() ;

    QVector<QPair<QString, QVector<CourseInfo>>>& getPastSemestersCourses() ;
    void archiveCurrentCourses(QString year);
    QJsonObject toJson() const;
    static UserInfo fromJson(const QJsonObject &obj);
    void setUsername(const QString &name);

    int getTotalUsedPoints() const ;
    int getRemainingPoints() const ;
    int getPointForCourse(const QString &courseCode) const ;
    void setPointForCourse(const QString &courseCode, int points) ;
    bool IsTeacher = false;


private:
    QString username;
    QString password;
    QString grade;
    QString college;
    QString Index;
    QString Realname;

    QMap<QString, int> courseVotes;

    QVector<CourseInfo> currentCourses;
    QVector<CourseInfo> favorites;
    QVector<QPair<QString, QVector<CourseInfo>>> pastSemestersCourses;
};
