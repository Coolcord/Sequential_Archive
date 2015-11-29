#include "Main_Window.h"
#include "Manager_Common_Strings.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include <QApplication>
#include <QPluginLoader>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //Load the sequential archive plugin
    Sequential_Archive_Interface *sequentialArchivePlugin = NULL;
    QPluginLoader *pluginLoader = new QPluginLoader(Common_Strings::PLUGIN_NAME + Common_Strings::PLUGIN_EXTENSION);
    QObject *validPlugin = pluginLoader->instance();
    if (validPlugin) sequentialArchivePlugin = qobject_cast<Sequential_Archive_Interface*>(validPlugin);
    if (!validPlugin || !sequentialArchivePlugin) {
        QMessageBox::critical(NULL, Common_Strings::MANAGER_NAME, "Unable to locate " + Common_Strings::PLUGIN_NAME + Common_Strings::PLUGIN_EXTENSION + "!",
                                  Common_Strings::OK);
        return 1;
    }

    //Display the main window
    MainWindow w(sequentialArchivePlugin);
    w.show();
    return a.exec();
}

