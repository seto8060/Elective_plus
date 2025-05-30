#include "CourseInfo.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>

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
    course.Max_person = obj.value("maxperson").toInt();
    course.Now_person = obj.value("nowperson").toInt();
    course.obj = obj;
    return course;
}
QJsonObject CourseInfo::toJson() const {
    QJsonObject obj;
    obj.insert("index", index);
    obj.insert("code", code);
    obj.insert("name", name);
    obj.insert("unit", unit);
    obj.insert("type", type);
    obj.insert("class", classNumber);
    obj.insert("score", score);
    obj.insert("week", week);

    QJsonArray timeArr;
    for (const auto &t : timeList)
        timeArr.append(t);
    obj.insert("time", timeArr);

    QJsonArray teacherArr;
    for (const auto &t : teacherList)
        teacherArr.append(t);
    obj.insert("teacher", teacherArr);

    obj.insert("info", info);
    obj.insert("first-class", firstClass);
    obj.insert("chinese-intro", chineseIntro);
    obj.insert("english-intro", englishIntro);
    QVector<int> indices = {2533,2781,2778,2568,2782,2696,2783};//for test
    obj.insert("maxperson", 90);
    obj.insert("nowperson", indices.contains(index.toInt()) ? 100 : 90);

    return obj;
}
bool saveCoursesToFile(const QString &filename, const QList<CourseInfo> &courses) {
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
