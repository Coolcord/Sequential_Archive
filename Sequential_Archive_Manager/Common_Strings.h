#ifndef COMMON_STRINGS
#define COMMON_STRINGS

#include <QString>

namespace Common_Strings {
    #ifdef Q_OS_WIN32
    const static QString STRING_PLUGIN_EXTENSION = ".dll"; //Windows uses .dll files
    const static QString STRING_NEW_LINE = "\r\n";
    #else
    const static QString STRING_PLUGIN_EXTENSION = ".so"; //Unix uses .so files
    const static QString STRING_NEW_LINE = "\n";
    #endif

    const static QString STRING_SEQUENTIAL_ARCHIVE = "Sequential Archive";
    const static QString STRING_SEQUENTIAL_ARCHIVE_MANAGER = STRING_SEQUENTIAL_ARCHIVE+" Manager";
    const static QString STRING_SAM = "SAM";
    const static QString STRING_EXTENSION = ".sa";
    const static QString STRING_PLUGIN_LOCATION = "./Plugins/Sequential_Archive"+STRING_PLUGIN_EXTENSION;
    const static QString STRING_OK = "OK";
    const static QString STRING_EXTENSION_FILTERS = STRING_SEQUENTIAL_ARCHIVE+"s (*"+STRING_EXTENSION+")\nAll files (*.*)";
}

#endif // COMMON_STRINGS

