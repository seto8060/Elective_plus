#include "CourseInfo.h"
#include <QJsonObject>
#include <QJsonArray>
#include<QJsonDocument>
#include <QFile>

CourseInfo parseCourseFromJson(const QJsonObject &obj) {
    CourseInfo course;
    course.index = obj.value("index").toString();
    course.code = obj.value("code").toString();
    course.name = obj.value("name").toString();
    course.unit = obj.value("unit").toString();
    course.type = obj.value("type").toString();
    course.classNumber = obj.value("class").toString();
    course.score = obj.value("score").toString();
    course.week = obj.value("week").toString();

    QJsonArray timeArr = obj.value("time").toArray();
    for (const auto &v : timeArr)
        course.timeList.append(v.toString());

    QJsonArray teacherArr = obj.value("teacher").toArray();
    for (const auto &v : teacherArr)
        course.teacherList.append(v.toString());

    course.info = obj.value("info").toString();
    course.firstClass = obj.value("first-class").toString();
    course.chineseIntro = obj.value("chinese-intro").toString();
    course.englishIntro = obj.value("english-intro").toString();
    course.obj = obj;
    return course;
}
QJsonObject CourseInfo::toJson() const {
    return obj;
}
static bool saveCoursesToFile(const QString &filename, const QList<CourseInfo> &courses) {
    QJsonArray arr;
    for (const auto &c : courses)
        arr.append(c.toJson());

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
