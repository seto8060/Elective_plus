#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include "userinfo.h"

class TeacherWindow : public QWidget {
    Q_OBJECT

public:
    explicit TeacherWindow(UserInfo *userinfo, QWidget *parent = nullptr);

private slots:
    void showMainPage();
    void showSubPage(const QString &pageName);

private:
    QWidget* createMainPage();
    QWidget* createSubPage(const QString &pageName);

    QStackedWidget *stackedWidget;
    QMap<QString, QWidget*> subPages;
};
/*
教务界面设计：
做成类似校内门户的界面
1.加/减课/修改选课信息
2.手工选课
3.开始/结束学期
4.抽签！
5.导出：每门课的选课信息、每门课的测评信息
可选功能：
批量操作用户
*/
