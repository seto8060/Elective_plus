#include "CourseLoader.h"
#include "CourseInfo.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>

extern CourseInfo parseCourseFromJson(const QJsonObject &obj);

QVector<CourseInfo> loadCoursesFromJsonFile(const QString &filePath) {
    QVector<CourseInfo> courseList;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开课程文件：" << filePath;
        return courseList;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        qWarning() << "课程文件格式错误";
        return courseList;
    }

    for (const QJsonValue &val : doc.array()) {
        if (val.isObject()) {
            courseList.append(parseCourseFromJson(val.toObject()));
        }
    }
    saveCoursesToFile("courses.json",courseList);
    return courseList;
}
