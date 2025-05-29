#include "TeacherInfo.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

TeacherInfo::TeacherInfo(QObject *parent): QObject(parent){
    QFile file("teacher_state.json");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject obj = doc.object();

        auto readTerm = [](const QJsonObject &o) -> Term {
            Term t(-1,-1);
            t.startYear = o["startYear"].toInt();
            t.semester = o["semester"].toInt();
            return t;
        };

        currentTerm = readTerm(obj["currentTerm"].toObject());
        enrollmentTerm = readTerm(obj["enrollmentTerm"].toObject());
        upcomingTerm = readTerm(obj["upcomingTerm"].toObject());
        lastEnrollmentEnded = readTerm(obj["lastEnrollmentEnded"].toObject());
        HasDoneLottery = obj["HasLottery"].toBool();
        verifycode = obj["VerifyCode"].toInt();
    }
}

void TeacherInfo::save() {
    QFile file("teacher_state.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法写入 teacher_state.json";
        return;
    }

    auto writeTerm = [](const Term &t) -> QJsonObject {
        QJsonObject o;
        o["startYear"] = t.startYear;
        o["semester"] = t.semester;
        return o;
    };

    QJsonObject obj;
    obj["currentTerm"] = writeTerm(currentTerm);
    obj["enrollmentTerm"] = writeTerm(enrollmentTerm);
    obj["upcomingTerm"] = writeTerm(upcomingTerm);
    obj["lastEnrollmentEnded"] = writeTerm(lastEnrollmentEnded);
    obj["HasLottery"] = HasDoneLottery;
    obj["VerifyCode"] = verifycode;

    QJsonDocument doc(obj);
    file.write(doc.toJson());
    file.close();
}
