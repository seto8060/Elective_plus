#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include "userinfo.h"
#include "courseloader.h"
#include "coursecomment.h"


class HistoryPage : public QWidget {
    Q_OBJECT
public:
    HistoryPage(UserInfo* user, QStackedWidget *stack, QVector<courseComment>* allcourseptr,QWidget *parent = nullptr);
private:
    UserInfo* user;
    QStackedWidget* stack;
};
