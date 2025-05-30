#include "commentloader.h"
#include  "coursecomment.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>


QVector<courseComment> loadCommentsFromJsonFile(const QString &filePath) {
    QVector<courseComment> courseList;
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
            courseList.append(parseCommentsFromJson(val.toObject()));
        }
    }
    return courseList;
}
