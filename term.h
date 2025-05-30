#pragma once
#include <QString>

struct Term {
    int startYear = 0, semester = 1;
    Term(int startYear, int semester):startYear(startYear),semester(semester){}
    bool operator==(const Term &other) const {
        return startYear == other.startYear && semester == other.semester;
    }

    bool operator!=(const Term &other) const {
        return !(*this == other);
    }

    bool operator<(const Term &other) const {
        if (startYear != other.startYear)
            return startYear < other.startYear;
        return semester < other.semester;
    }

    QString toString() const {
        if (semester == 0) return QString("无");
        return QString("%1-%2学年第%3学期")
            .arg(startYear)
            .arg(startYear + 1)
            .arg(semester);
    }

    Term nextTerm() {
        Term t(this->startYear, this->semester);
        if (t.semester == 3) t.semester = 1, t.startYear ++;
        else t.semester ++;
        return t;
    }

    bool isValid() const {
        return startYear > 0 && semester >= 1 && semester <= 3;
    }
};
