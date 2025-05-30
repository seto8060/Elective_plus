// CourseDetailPage.h
#ifndef COURSEDETAILPAGE_H
#define COURSEDETAILPAGE_H

#include <QWidget>
#include<QLabel>
#include<QPushButton>
#include<QTextEdit>
#include<QVBoxLayout>
#include "CourseInfo.h"
#include "courseComment.h"

class QLabel;
class QPushButton;

class CourseDetailPage : public QWidget {
    Q_OBJECT
public:
    explicit CourseDetailPage(const CourseInfo& course, const courseComment& comment,QWidget *parent = nullptr);

signals:
    void backRequested();
    void enrollRequested(const QString& courseCode);

private:
    void setupUI(const CourseInfo& course,const courseComment& comment);

    QLabel* createSectionTitle(const QString& text);
    QWidget* createInfoSection(const CourseInfo& course);
    QWidget* createIntroSection(const CourseInfo& course);
    QLabel *m_titleLabel;
    QLabel *m_codeLabel;
    QLabel *m_teachersLabel;
    QLabel *m_timeLabel;
    QLabel *m_locationLabel;
    QLabel *m_descriptionLabel;
    QPushButton *m_enrollButton;
    QPushButton *m_backButton;
    QVector<comment> m_comments;
    QVBoxLayout* m_commentsLayout;
    int m_currentPage = 0;
    const int COMMENTS_PER_PAGE = 5;
    
    QWidget* createCommentsSection(const courseComment& comment);
    void updateCommentsDisplay();
};

#endif // COURSEDETAILPAGE_H
