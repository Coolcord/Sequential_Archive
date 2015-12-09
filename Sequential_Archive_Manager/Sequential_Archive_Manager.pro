#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T10:55:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sequential_Archive_Manager
TEMPLATE = app


SOURCES += main.cpp \
    Main_Window.cpp

HEADERS  += \
    ../Sequential_Archive/Common_Strings.h \
    Manager_Common_Strings.h \
    Main_Window.h \
    Archive_Mode.h \
    Archive_Mode.h

FORMS    += mainwindow.ui
