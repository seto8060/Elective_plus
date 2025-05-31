#include "timetable.h"
#include <QPixmap>
#include <QRegularExpression>
#include <QLabel>

CustomTimetableWidget::CustomTimetableWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(1180, 800);
    setupBackground();
}

void CustomTimetableWidget::setupBackground() {
    bgLabel = new QLabel(this);
    bgLabel->setPixmap(QPixmap(":/resources/pictures/table.svg"));
    bgLabel->setScaledContents(true);
    bgLabel->setFixedSize(this->size());

    overlayLayer = new QWidget(this);
    overlayLayer->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlayLayer->setStyleSheet("background: transparent;");
    overlayLayer->setFixedSize(this->size());
}

void CustomTimetableWidget::setCourses(const QVector<CourseInfo> &courses) {
    displayCourses(courses);
}

void CustomTimetableWidget::displayCourses(const QVector<CourseInfo> &courses) {
    const auto children = overlayLayer->findChildren<QLabel*>(QString(), Qt::FindDirectChildrenOnly);
    for (QLabel *label : children) {
        label->deleteLater();
    }

    QStringList days = {"一", "二", "三", "四", "五", "六", "日"};
    for (int i = 0; i < 7; ++i) {
        QLabel *dayLabel = new QLabel("星期" + days[i], this);
        dayLabel->setGeometry(100 + i * 150, 50, 140, 30);
        dayLabel->setAlignment(Qt::AlignCenter);
        dayLabel->setStyleSheet("font-weight: bold; color: #444; font-size: 14px;");
    }
    for (int i = 1; i <= 12; ++i) {
        QLabel *rowLabel = new QLabel(QString("第%1节").arg(i), this);
        rowLabel->setGeometry(10, 100 + (i - 1) * 52, 80, 52);  // 左侧纵向
        rowLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        rowLabel->setStyleSheet("font-size: 13px; color: #333;");
    }
    for (const CourseInfo &course : courses) {
        for (const QString &t : course.timeList) {
            QRegularExpression r(R"(星期([一二三四五六日天])\(第(\d+)节-第(\d+)节\)(?:\((单|双)\))?)");
            QRegularExpressionMatch m = r.match(t);
            if (!m.hasMatch()) continue;

            QString day = "星期" + m.captured(1);
            QString weekFilter = m.captured(4);

            QString nameText = course.name;
            nameText += "（"+course.week + "）";
            if (!weekFilter.isEmpty()) {
                nameText += QString("（%1）").arg(weekFilter);
            }
            int start = m.captured(2).toInt();
            int end = m.captured(3).toInt();
            // qDebug() << start << end << Qt::endl;
            QPoint Start_Pos = getCoursePosition(day, start), End_Pos = getCoursePosition(day, end);
            // QPoint pos = QPoint(Start_Pos.x(),End_Pos.y());
            QLabel *label = new QLabel(nameText + "\n" + course.teacherList.join(", "), overlayLayer);
            label->setStyleSheet("background-color: rgba(200,220,255,180); border: 1px solid #444; font-size: 12px;");
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignCenter);
            label->setGeometry(Start_Pos.x(), Start_Pos.y(), 140, 52 * (end-start+1)-5);
            QString key = course.name + course.teacherList.join("");
            int hue = qAbs(qHash(key)) % 360;
            QColor color = QColor::fromHsv(hue, 80, 255, 200);
            label->setStyleSheet(QString("background-color: %1; border-radius: 6px;")
                                .arg(color.name(QColor::HexArgb)));
        }
    }
}

QPoint CustomTimetableWidget::getCoursePosition(const QString &day, int section) {
    int col = getDayColumn(day);
    int x = 100 + col * 150;
    int y = 100 + (section - 1) * 52;
    return QPoint(x, y);
}

int CustomTimetableWidget::getDayColumn(const QString &day) {
    if (day.contains("一")) return 0;
    if (day.contains("二")) return 1;
    if (day.contains("三")) return 2;
    if (day.contains("四")) return 3;
    if (day.contains("五")) return 4;
    if (day.contains("六")) return 5;
    if (day.contains("日") || day.contains("天")) return 6;
    return -1;
}
