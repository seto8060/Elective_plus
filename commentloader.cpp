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

#include<QDir>
#include<QStandardPaths>
    void saveCommentToJson(QVector<courseComment> *comments) {
        // 确保数据非空
        if (comments->isEmpty()) {
            qDebug() << "Error: No data to save!";
            return;
        }

        // 获取标准路径
        QString filePath = "comments.json";
        qDebug() << "Writing to file:" << filePath;

        // 创建目录（如果不存在）
        QDir dir(QFileInfo(filePath).absolutePath());
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // 打开文件
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qDebug() << "Error: Failed to open file for writing!" << file.errorString();
            return;
        }

        // 构建 JSON 结构
        QJsonArray rootArray;
        for (const auto &course : *comments) {
            QJsonObject courseObj;
            courseObj["code"] = course.code;

            QJsonArray commentsArray;
            for (const auto &comment : course.comments) {
                QJsonObject singleComment;
                singleComment["content"] = comment.content;
                singleComment["priority"] = comment.priority;
                singleComment["critic"] = comment.critic;
                singleComment["semester"] = comment.semester;
                commentsArray.append(singleComment);
            }

            courseObj["comments"] = commentsArray;
            rootArray.append(courseObj);
        }

        // 写入文件
        QJsonDocument doc(rootArray);
        QByteArray data = doc.toJson(QJsonDocument::Indented);

        if (file.write(data) != data.size()) {
            qDebug() << "Error: Failed to write all data to file!";
        }

        file.close();
        qDebug() << "Success: Data written to" << filePath;

    }

