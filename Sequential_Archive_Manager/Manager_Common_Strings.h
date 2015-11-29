#ifndef MANAGER_COMMON_STRINGS_H
#define MANAGER_COMMON_STRINGS_H

#include "../Sequential_Archive/Common_Strings.h"
#include <QString>

namespace Common_Strings {
    const static QString MANAGER_NAME = FORMAT_NAME + " Manager";
    const static QString PLUGIN_NAME = "Sequential_Archive";
    const static QString OK = "OK";

    #ifdef Q_OS_WIN32
    const static QString PLUGIN_EXTENSION = ".dll"; //Windows uses .dll files
    const static QString NEW_LINE = "\r\n"; //Qt's endl seems to use only \n for some reason... I'll look into this later
    #else
    const static QString PLUGIN_EXTENSION = ".so"; //Unix uses .so files
    const static QString NEW_LINE = "\n";
    #endif
}

#endif // MANAGER_COMMON_STRINGS_H

