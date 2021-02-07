#include "Main_Window.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include "Command_Line_Runner.h"
#include "Common_Strings.h"
#include "Error_Messages.h"
#include <QApplication>
#include <QPluginLoader>
#include <QTime>

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(QTime::currentTime().msecsSinceStartOfDay()));
    QApplication a(argc, argv);
    #ifdef Q_OS_WIN32
    a.setStyle("fusion"); //use fusion theme on Windows
    #endif

    //Load the Hexagon Plugin
    QString unableToLoadPluginMessage = Common_Strings::STRING_SEQUENTIAL_ARCHIVE_MANAGER+" is unable to load the backend plugin! Make sure the plugin is in the plugins folder!";
    QPluginLoader *pluginLoader = new QPluginLoader(Common_Strings::STRING_PLUGIN_LOCATION);
    QObject *validPlugin = pluginLoader->instance();
    Error_Messages errorMessages;
    if (!validPlugin) {
        errorMessages.Show_Error(unableToLoadPluginMessage);
        return 1;
    }
    Sequential_Archive_Interface *sequentialArchivePlugin = qobject_cast<Sequential_Archive_Interface*>(validPlugin);
    if (!sequentialArchivePlugin) {
        errorMessages.Show_Error(unableToLoadPluginMessage);
        return 1;
    }

    //Run Via Command Line if Necessary
    if (argc > 1) return Command_Line_Runner(argc, argv, sequentialArchivePlugin).Run_Commands();

    Main_Window w(nullptr, sequentialArchivePlugin, &errorMessages);
    w.show();
    return a.exec();
}
