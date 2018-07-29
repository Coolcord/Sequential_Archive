#include "Main_Window.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include "Common_Strings.h"
#include "Error_Messages.h"
#include <QApplication>
#include <QPluginLoader>
#include <QTime>

int main(int argc, char *argv[]) {
    //Use Plastique Theme on Windows
    #ifdef Q_OS_WIN32
    QApplication::setStyle("plastique");
    #endif

    QApplication a(argc, argv);
    qsrand(QTime::currentTime().msecsSinceStartOfDay());

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

    Main_Window w(NULL, sequentialArchivePlugin, &errorMessages);
    w.show();
    return a.exec();
}
