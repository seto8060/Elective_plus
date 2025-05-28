#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include "userinfo.h"

class HistoryPage : public QWidget {
    Q_OBJECT
public:
    HistoryPage(UserInfo* user, QStackedWidget *stack, QWidget *parent = nullptr);
private:
    UserInfo* user;
    QStackedWidget* stack;
};
