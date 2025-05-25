#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>

class CourseInfo {
public:
    QString index;
    QString code;
    QString name;
    QString unit;
    QString type;
    QString classNumber;
    QString score;
    QString week;
    QStringList timeList;
    QStringList teacherList;
    QString info;
    QString firstClass;
    QString chineseIntro;
    QString englishIntro;
    int Max_person = 90;
    int Now_person = 100;
    QJsonObject obj;
    QJsonObject toJson() const;
};
CourseInfo parseCourseFromJson(const QJsonObject &obj);
