#ifndef COMMENTLOADER_H
#define COMMENTLOADER_H

#include "coursecomment.h"
#include <QVector>
#include <QString>

QVector<courseComment> loadCommentsFromJsonFile(const QString &filePath);
void saveCommentToJson(QVector<courseComment> *comments);
#endif // COMMENTLOADER_H

