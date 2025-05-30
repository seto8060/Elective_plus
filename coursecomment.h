#ifndef COURSECOMMENT_H
#define COURSECOMMENT_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include<QJsonArray>

class comment{
public:
    //int index;
    QString critc;
    int priority;//总评
    QString content;
    
};

class courseComment {
public:
    QString code;
    QVector<comment> comments;
    QJsonObject obj;
    QJsonObject toJson() const;


};
courseComment parseCommentsFromJson(const QJsonObject &obj); 


#endif // COURSECOMMENT_H
