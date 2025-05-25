#pragma once

#include "CourseInfo.h"
#include <QVector>
#include <QString>

QVector<CourseInfo> loadCoursesFromJsonFile(const QString &filePath);
