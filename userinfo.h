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
             const QString &college);

    QString getUsername() const;
    QString getPassword() const;
    QString getGrade() const;
    QString getCollege() const;

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

private:
    QString username;
    QString password;
    QString grade;
    QString college;

    QVector<CourseInfo> currentCourses;
    QVector<CourseInfo> favorites;
    QVector<QPair<QString, QVector<CourseInfo>>> pastSemestersCourses;
};
