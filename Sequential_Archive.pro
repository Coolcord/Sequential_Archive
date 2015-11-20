#-------------------------------------------------
#
# Project created by QtCreator 2015-09-13T16:19:56
#
#-------------------------------------------------

QT       += core

TARGET = Sequential_Archive
TEMPLATE = lib
CONFIG += plugin c++11 pendantic Wall

DEFINES += Sequential_Archive_LIBRARY

SOURCES += \
    Sequential_Archive.cpp \
    Packer.cpp \
    Scrambler.cpp \
    Reader.cpp

HEADERS += \
    Sequential_Archive.h \
    Sequential_Archive_Interface.h \
    Packer.h \
    Scrambler.h \
    Common_Strings.h \
    Common_Data.h \
    Reader.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
