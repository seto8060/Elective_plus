#include "coursecomment.h"

courseComment parseCommentsFromJson(const QJsonObject &obj){
    courseComment course;

    course.code = obj.value("code").toString();
    QJsonArray comArr=obj.value("comments").toArray();
    for(const auto &v:comArr){
        comment c;
        QJsonObject item=v.toObject();
        c.critic=item.value("critic").toString();
        c.content=item.value("content").toString();
        c.priority=item.value("priority").toInt();
        c.semester=item.value("semestr").toString();
        c.hwPrefer=item.value("hwPrefer").toInt();
        c.listenPrefer=item.value("listenPrefer").toInt();
        c.scorePrefer=item.value("scorePrefer").toInt();

        QJsonArray teacherArray = item.value("teacher").toArray();
        QVector<QString> teacherVector;

        for (const QJsonValue &value : teacherArray) {
            teacherVector.append(value.toString());
        }

        c.teacher = teacherVector;
        course.comments.append(c);
    }
    course.obj = obj;
    return course;
}

QJsonObject courseComment::toJson() const {
    return obj;
}
