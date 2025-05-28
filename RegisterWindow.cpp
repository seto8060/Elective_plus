#include "RegisterWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include<QCompleter>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>

RegisterWindow::RegisterWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("用户注册");
    resize(350, 300);

    auto *layout = new QVBoxLayout(this);

    Name_Box = new QLineEdit(this);
    Name_Box->setPlaceholderText("请输入用户名");

    Pass_Box = new QLineEdit(this);
    Pass_Box->setPlaceholderText("请输入密码");
    Pass_Box->setEchoMode(QLineEdit::Password);

    verifyLabel = new QLabel("请输入验证码：", this);
    verifyBox = new QLineEdit(this);
    verifyBox->setPlaceholderText("验证码");
    verifyLabel->setVisible(false);
    verifyBox->setVisible(false);

    Confirm_Pass_Box = new QLineEdit(this);
    Confirm_Pass_Box->setPlaceholderText("请确认密码");
    Confirm_Pass_Box->setEchoMode(QLineEdit::Password);

    rolebox = new QComboBox(this);
    rolebox->addItems({"学生", "教务"});

    gradeBox = new QComboBox(this);
    gradeBox->addItems({"2020级", "2021级", "2022级", "2023级", "2024级"});
    QStringList colleges = {
                            "数学科学学院",
                            "物理学院",
                            "化学与分子工程学院",
                            "生命科学学院",
                            "地球与空间科学学院",
                            "心理与认知科学学院",
                            "新闻与传播学院",
                            "中国语言文学系",
                            "历史学系",
                            "考古文博学院",
                            "哲学系",
                            "国际关系学院",
                            "经济学院",
                            "光华管理学院",
                            "法学院",
                            "信息管理系",
                            "社会学系",
                            "政府管理学院",
                            "英语语言文学系",
                            "外国语学院",
                            "马克思主义学院",
                            "体育教研部",
                            "艺术学院",
                            "元培学院",
                            "信息科学技术学院",
                            "国家发展研究院",
                            "工学院",
                            "城市与环境学院",
                            "环境科学与工程学院"
    };

    collegeBox = new QComboBox(this);
    collegeBox->setEditable(true);
    collegeBox->addItems(colleges);
    collegeBox->setCompleter(new QCompleter(colleges, this));

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red");
    errorLabel->setVisible(false);

    Register_Button = new QPushButton("注册", this);
    layout->addWidget(Name_Box);
    layout->addWidget(Pass_Box);
    layout->addWidget(Confirm_Pass_Box);
    layout->addWidget(rolebox);
    layout->addWidget(gradeBox);
    layout->addWidget(collegeBox);
    layout->addWidget(verifyLabel);
    layout->addWidget(verifyBox);
    layout->addWidget(Register_Button);
    layout->addWidget(errorLabel);

    connect(Register_Button, &QPushButton::clicked, this, &RegisterWindow::Register);

    connect(rolebox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        bool isStudent = (rolebox->currentText() == "学生");
        gradeBox->setVisible(isStudent);
        collegeBox->setVisible(isStudent);
        verifyLabel->setVisible(!isStudent);
        verifyBox->setVisible(!isStudent);
    });
}

void RegisterWindow::Register() {
    QString user = Name_Box->text();
    QString pass1 = Pass_Box->text();
    QString pass2 = Confirm_Pass_Box->text();

    if (user.isEmpty() || pass1.isEmpty() || pass2.isEmpty()) {
        errorLabel->setText("请填写所有字段！");
        errorLabel->setVisible(true);
        return;
    }

    if (pass1 != pass2) {
        errorLabel->setText("两次输入的密码不一致！");
        errorLabel->setVisible(true);
        return;
    }

    if (rolebox->currentText() == "教务") {
        if (verifyBox->text() != "1024") {
            errorLabel->setText("验证码错误!");
            errorLabel->setVisible(true);
            return;
        }
        else {
            newUser = UserInfo(user,pass1, "","",true);
            // qDebug() << newUser.IsTeacher ;
            accept();
        }
        return ;
    }
    QString confirmText = QString("我超！盒！请问是%1%2的%3同学吗？")
                              .arg(collegeBox->currentText())
                              .arg(gradeBox->currentText())
                              .arg(Name_Box->text());

    int reply = QMessageBox::question(this, "注册确认", confirmText,
                                      QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qDebug() << 1;
        newUser = UserInfo(user, pass1, gradeBox->currentText(), collegeBox->currentText(), false);
        accept();
    }
}

QString RegisterWindow::getUsername() const {
    return Name_Box->text();
}

QString RegisterWindow::getPassword() const {
    return Pass_Box->text();
}

QString RegisterWindow::getRole() const {
    return rolebox->currentText();
}

const UserInfo& RegisterWindow::getUserInfo() const {
    return newUser;
}
