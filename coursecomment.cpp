#include "coursecomment.h"

courseComment parseCommentsFromJson(const QJsonObject &obj){
    courseComment course;

    course.code = obj.value("code").toString();
    QJsonArray comArr=obj.value("comments").toArray();
    for(const auto &v:comArr){
        comment c;
        QJsonObject item=v.toObject();
        c.critc=item.value("critic").toString();
        c.content=item.value("content").toString();
        c.priority=item.value("priority").toInt();
        course.comments.append(c);
    }
    course.obj = obj;
    return course;
}

QJsonObject courseComment::toJson() const {
    return obj;
}
