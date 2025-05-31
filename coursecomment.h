#ifndef COURSECOMMENT_H
#define COURSECOMMENT_H

#include "term.h"
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include<QJsonArray>

class comment{
public:
    //int index;
    QString critic;
    int priority;//总评
    int scorePrefer;
    int hwPrefer;
    int listenPrefer;
    QString content;
    QString semester;

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
