#include "UserInfo.h"
#include "CourseLoader.h"
#include "CourseInfo.h"
#include <QJsonArray>

UserInfo::UserInfo() {}

UserInfo::UserInfo(const QString &username,
                   const QString &password,
                   const QString &grade,
                   const QString &college,
                   const QString &Index,
                   const QString &Realname,
                   const bool &IsTeacher)
    : username(username),
    password(password),
    grade(grade),
    college(college),
    Index(Index),
    Realname(Realname),
    IsTeacher(IsTeacher){
        QVector<CourseInfo> All_courses = loadCoursesFromJsonFile(":/resources/resources/courses.json");
        QVector<int> indices = {2533,2781,2778,2568,2782,2696,2783};
        // currentCourses;
        for (int idx : indices) {
            if (idx >= 0 && idx < All_courses.size()) {
                currentCourses.append(All_courses[idx-1]);
                setPointForCourse(All_courses[idx-1].code,0);
            }
        }
        // currentCourses = All_courses.mid(1,8);//for test
    }

QString UserInfo::getUsername() const {
    return username;
}

QString UserInfo::getPassword() const {
    return password;
}

QString UserInfo::getGrade() const {
    return grade;
}

QString UserInfo::getCollege() const {
    return college;
}

QString UserInfo::getIndex() const{
    return Index;
}

QString UserInfo::getRealname() const{
    return Realname;
}

int UserInfo::getTotalUsedPoints() const {
    int sum = 0;
    for (int p : courseVotes.values())
        sum += p;
    return sum;
}

int UserInfo::getRemainingPoints() const {
    return 99 - getTotalUsedPoints();
}

int UserInfo::getPointForCourse(const QString &courseCode) const {
    return courseVotes.value(courseCode, 0);
}

void UserInfo::setPointForCourse(const QString &courseCode, int points) {
    courseVotes[courseCode] = points;
}

void UserInfo::setPassword(const QString &pwd) {
    password = pwd;
}

void UserInfo::setGrade(const QString &g) {
    grade = g;
}

void UserInfo::setCollege(const QString &c) {
    college = c;
}

QVector<CourseInfo>& UserInfo::getCurrentCourses() {
    return currentCourses;
}

QVector<CourseInfo>& UserInfo::getFavorites() {
    return favorites;
}

QVector<QPair<QString, QVector<CourseInfo>>>& UserInfo::getPastSemestersCourses() {
    return pastSemestersCourses;
}

void UserInfo::archiveCurrentCourses(QString year) {
    if (!currentCourses.isEmpty()) {
        pastSemestersCourses.append(qMakePair(year,currentCourses));
        currentCourses.clear();
    }
}

QJsonObject UserInfo::toJson() const {
    QJsonObject obj;
    if (IsTeacher){
        // qDebug() << 12;
        obj["password"] = password;
        obj["IsTeacher"] = true;
        // qDebug() << obj;
        return obj;
    }
    obj["IsTeacher"] = false;
    obj["password"] = password;
    obj["grade"] = grade;
    obj["college"] = college;
    obj["index"] = Index;
    obj["realname"] = Realname;
    QJsonObject voteObj;
    for (auto it = courseVotes.begin(); it != courseVotes.end(); ++it) {
        voteObj[it.key()] = it.value();
    }
    obj["courseVotes"] = voteObj;
    QJsonArray courseArray;
    for (const CourseInfo &course : currentCourses) {
        courseArray.append(course.toJson());
    }
    obj["currentCourses"] = courseArray;

    QJsonArray favoriteArray;
    for (const CourseInfo &course : favorites) {
        favoriteArray.append(course.toJson());
    }
    obj["favorites"] = favoriteArray;

    QJsonArray historyArray;
    for (const auto &pair : pastSemestersCourses) {
        QJsonObject semesterObj;
        semesterObj["semester"] = pair.first;

        QJsonArray courseListArray;
        for (const CourseInfo &course : pair.second) {
            courseListArray.append(course.toJson());
        }

        semesterObj["courses"] = courseListArray;
        historyArray.append(semesterObj);
    }
    obj["pastSemestersCourses"] = historyArray;

    return obj;
}

UserInfo UserInfo::fromJson(const QJsonObject &obj) {
    UserInfo u;
    if (obj["IsTeacher"].toBool() == true){
        u.IsTeacher = true;
        u.password = obj["password"].toString();
        return u;
    }
    u.password = obj["password"].toString();
    u.grade = obj["grade"].toString();
    u.college = obj["college"].toString();
    u.Index = obj["index"].toString();
    u.Realname = obj["realname"].toString();
    if (obj.contains("courseVotes") && obj["courseVotes"].isObject()) {
        QJsonObject voteObj = obj["courseVotes"].toObject();
        for (auto it = voteObj.begin(); it != voteObj.end(); ++it) {
            u.courseVotes[it.key()] = it.value().toInt();
        }
    }
    // qDebug() << 1;
    QJsonArray courseArray = obj["currentCourses"].toArray();
    for (const QJsonValue &val : courseArray) {
        u.currentCourses.append(parseCourseFromJson(val.toObject()));
    }

    QJsonArray favArray = obj["favorites"].toArray();
    for (const QJsonValue &val : favArray) {
        u.favorites.append(parseCourseFromJson(val.toObject()));
    }
    // qDebug() << 2;
    QJsonArray historyArray = obj["pastSemestersCourses"].toArray();
    for (const QJsonValue &val : historyArray) {
        QJsonObject semObj = val.toObject();
        QString semester = semObj["semester"].toString();

        QVector<CourseInfo> semCourses;
        QJsonArray semCourseArray = semObj["courses"].toArray();
        for (const QJsonValue &c : semCourseArray) {
            semCourses.append(parseCourseFromJson(c.toObject()));
        }

        u.pastSemestersCourses.append(qMakePair(semester, semCourses));
    }
    return u;
    // qDebug() << 3;
}

void UserInfo::setUsername(const QString &name) {
    username = name;
}
