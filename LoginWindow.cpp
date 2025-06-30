#include "LoginWindow.h"
#include "RegisterWindow.h"
#include "teacherinfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("选课系统登录");
    this->setStyleSheet(R"(
    QDialog {
        background-color: #f9f9f9;
    }

    QLineEdit, QComboBox {
        border: 1px solid #ccc;
        border-radius: 6px;
        padding: 6px;
        background-color: white;
        font-size: 14px;
    }

    QPushButton {
        background-color: #0078d7;
        color: white;
        border-radius: 6px;
        padding: 6px 12px;
        font-weight: bold;
    }

    QPushButton:hover {
        background-color: #005fa1;
    }

    QLabel#errorLabel {
        color: red;
        font-size: 12px;
    }
    )");

    auto *layout = new QVBoxLayout(this);

    Name_Box = new QLineEdit(this);
    Name_Box->setPlaceholderText("请输入用户名");
    QHBoxLayout *Pass_Layout = new QHBoxLayout();
    Pass_Box = new QLineEdit(this);
    Pass_Box->setEchoMode(QLineEdit::Password);
    Pass_Box->setPlaceholderText("请输入密码");

    QIcon Eye(":/resources/icon/eye.svg"), Eye_off(":/resources/icon/eye-off.svg");

    Display_Pass = new QPushButton();
    Display_Pass->setCheckable(true);
    Display_Pass->setObjectName("displayPassButton");

    Display_Pass->setStyleSheet(R"(
    #displayPassButton {
        background-color: transparent;
        border: none;
    }

    #displayPassButton:hover {
        background-color: #D3D3D3;
    }

    #displayPassButton:checked {
        background-color: #808080;
        border-radius: 4px;
    }

    #displayPassButton:checked:hover {
        background-color: #A9A9A9;
        border-radius: 4px;
    }
    )");
    Display_Pass->setFixedSize(28, 24);

    Display_Pass->setIcon(Eye);
    Display_Pass->setIconSize(QSize(20, 18));
    Display_Pass->setToolTip("显示/隐藏密码");

    connect(Display_Pass, &QPushButton::toggled, this, [=](bool checked) {
        if (checked) {
            Pass_Box->setEchoMode(QLineEdit::Normal);
            Display_Pass->setIcon(Eye_off);
        } else {
            Pass_Box->setEchoMode(QLineEdit::Password);
            Display_Pass->setIcon(Eye);
        }
    });

    Pass_Layout->addWidget(Pass_Box), Pass_Layout->addWidget(Display_Pass);
    roleBox = new QComboBox(this);
    roleBox->addItem("学生");
    roleBox->addItem("教务");

    verifyLabel = new QLabel("请输入令牌：", this);
    verifyBox = new QLineEdit(this);
    verifyBox->setPlaceholderText("令牌");
    verifyLabel->setVisible(false);
    verifyBox->setVisible(false);
    connect(roleBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        bool isStudent = (roleBox->currentText() == "学生");
        verifyLabel->setVisible(!isStudent);
        verifyBox->setVisible(!isStudent);
    });
    // if (roleBox->currentText() == "教务") {
    //     if (verifyBox->text() != "1024") {
    //         errorLabel->setText("验证码错误!");
    //         errorLabel->setVisible(true);
    //         return;
    //     }
    //     else {
    //         newUser = UserInfo(user,pass1, "","","","",true);
    //         // qDebug() << newUser.IsTeacher ;
    //         accept();
    //     }
    //     return ;
    // }

    Login_Button = new QPushButton("登录", this);
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red");
    errorLabel->setVisible(false);

    Register_Button = new QPushButton("注册",this);

    layout->addWidget(Name_Box);
    layout->addLayout(Pass_Layout);
    layout->addWidget(roleBox);
    layout->addWidget(verifyLabel);
    layout->addWidget(verifyBox);
    layout->addWidget(Login_Button);
    layout->addWidget(Register_Button);
    layout->addWidget(errorLabel);

    connect(Login_Button, &QPushButton::clicked, this, &LoginWindow::handleLogin);

    connect(Register_Button, &QPushButton::clicked, this, &LoginWindow::handleRegister);
}
bool loadUserFromJson(const QString &username, const QString &password, UserInfo *&user) {
    QFile file("users.json");
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    if (!root.contains(username)) return false;

    QJsonObject obj = root.value(username).toObject();
    QString savedPass = obj.value("password").toString();
    if (savedPass != password) return false;
    // if (obj.value("IsTeacher") != Role) return false;
    // qDebug() << 1;
    user = new UserInfo(UserInfo::fromJson(obj));
    // qDebug() << 1;
    user->setUsername(username);
    // qDebug() << 1;
    return true;
}

void LoginWindow::handleLogin() {
    QString username = Name_Box->text();
    QString password = Pass_Box->text();
    bool Role = roleBox->currentText() == "教务";
    if (Role == true){
        if (username != "admin" || password != "adminpass") {
            errorLabel->setText("用户名或密码错误！");
            errorLabel->setVisible(true);
            return ;
        }
        TeacherInfo *t = new TeacherInfo(this);
        // qDebug() << t->getverifycode();
        if (verifyBox->text() != QString::number(t->getverifycode())){
            errorLabel->setText("令牌校验错误！");
            errorLabel->setVisible(true);
            return;
        }
        // qDebug() << 1;
        UserInfo *user= new UserInfo(0);
        // qDebug() << user->getIndex();
        emit loginSuccess(user);
        this->close();
        return ;
    }
    UserInfo *user;
    if (loadUserFromJson(username, password, user)) {
        emit loginSuccess(user);
        this->close();
    } else {
        errorLabel->setText("用户名或密码错误！");
        errorLabel->setVisible(true);
    }
}

void saveUserToJson(UserInfo *user) {
    QFile file("users.json");
    QJsonObject root;

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject())
            root = doc.object();
        file.close();
    }

    QJsonObject newUser = user->toJson();
    root.insert(user->getUsername(), newUser);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void LoginWindow::handleRegister(){
        RegisterWindow reg(this);
        if (reg.exec() == QDialog::Accepted) {
            UserInfo *newUser = new UserInfo(reg.getUserInfo());
            saveUserToJson(newUser);
            QMessageBox::information(this, "注册成功", "注册成功，请登录！");
        }
}
bool LoginWindow::validateCredentials(const QString &user, const QString &pass, const QString &role) {
    QFile file("users.json");
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    QJsonObject roleObj = root.value(role).toObject();
    QString savedPass = roleObj.value(user).toString();

    return !savedPass.isEmpty() && savedPass == pass;
}
