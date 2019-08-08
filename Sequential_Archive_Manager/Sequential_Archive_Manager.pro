#-------------------------------------------------
#
# Project created by QtCreator 2018-06-12T16:19:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sequential_Archive_Manager
TEMPLATE = app
CONFIG += c++11 pendantic Weverything console

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        Main_Window.cpp \
    Viewer.cpp \
    Error_Messages.cpp \
    Command_Line_Runner.cpp

HEADERS += \
        Main_Window.h \
    Viewer.h \
    Common_Strings.h \
    Error_Messages.h \
    Version.h \
    Command_Line_Runner.h

FORMS += \
        Main_Window.ui \
    Viewer.ui

RC_FILE = Sequential_Archive.rc

OTHER_FILES +=
