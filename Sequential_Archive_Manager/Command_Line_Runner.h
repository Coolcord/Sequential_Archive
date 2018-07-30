#ifndef COMMAND_LINE_RUNNER_H
#define COMMAND_LINE_RUNNER_H

#include <QStringList>

class Sequential_Archive_Interface;

class Command_Line_Runner {
public:
    Command_Line_Runner(int argc, char *argv[], Sequential_Archive_Interface *sequentialArchivePlugin);
    ~Command_Line_Runner();
    int Run_Commands();

private:
    int Parse_Command();
    void Remove_Command_Identifier_Characters(QString &command);
    int Show_Error(int command, int errorCode);
    void Show_Help();
    void Show_Invalid_Number_Of_Arguments_Message();

    QStringList *args;
    Sequential_Archive_Interface *sequentialArchivePlugin;
    QMap<QString, int> *commandMap;
};

#endif // COMMAND_LINE_RUNNER_H
