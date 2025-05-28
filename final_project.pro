QT       += core gui
QT += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LoginWindow.cpp \
    RegisterWindow.cpp \
    courseinfo.cpp \
    courselistwidget.cpp \
    courseloader.cpp \
    historypage.cpp \
    homepage.cpp \
    main.cpp \
    mainwindow.cpp \
    teacherwindow.cpp \
    timetable.cpp \
    timetablepage.cpp \
    userinfo.cpp

HEADERS += \
    LoginWindow.h \
    RegisterWindow.h \
    courseinfo.h \
    courselistwidget.h \
    courseloader.h \
    historypage.h \
    homepage.h \
    mainwindow.h \
    teacherwindow.h \
    timetable.h \
    timetablepage.h \
    userinfo.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    final_project_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
