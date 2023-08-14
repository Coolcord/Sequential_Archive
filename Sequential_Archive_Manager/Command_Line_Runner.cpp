#include "Command_Line_Runner.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include <QFileInfo>
#include <QDebug>

Command_Line_Runner::Command_Line_Runner(int argc, char *argv[], Sequential_Archive_Interface *sequentialArchivePlugin) {
    assert(sequentialArchivePlugin);
    this->sequentialArchivePlugin = sequentialArchivePlugin;
    this->args = new QStringList();
    for (int i = 0; i < argc; ++i) this->args->push_back(QString(argv[i]));

    //Populate the Possible Commands
    this->commandMap = new QMap<QString, int>();
    this->commandMap->insert("pack", 1);
    this->commandMap->insert("p", 1);
    this->commandMap->insert("unpack", 2);
    this->commandMap->insert("u", 2);
}

Command_Line_Runner::~Command_Line_Runner() {
    delete this->args;
    delete this->commandMap;
}

int Command_Line_Runner::Run_Commands() {
    switch (this->Parse_Command()) {
    default: assert(false); return 1;
    case 0: //Help
        this->Show_Help();
        return 0;
    case 1: //Pack
        if (this->args->size() == 3) {
            return this->Show_Error(1, this->sequentialArchivePlugin->Pack(this->args->at(2)));
        } else if (this->args->size() == 4) {
            return this->Show_Error(1, this->sequentialArchivePlugin->Pack(this->args->at(2), this->args->at(3)));
        } else {
            this->Show_Invalid_Number_Of_Arguments_Message();
            return 1;
        }
    case 2: //Unpack
        if (this->args->size() == 3) {
            return this->Show_Error(2, this->sequentialArchivePlugin->Unpack(this->args->at(2)));
        } else if (this->args->size() == 4) {
            return this->Show_Error(2, this->sequentialArchivePlugin->Unpack(this->args->at(2), this->args->at(3)));
        } else {
            this->Show_Invalid_Number_Of_Arguments_Message();
            return 1;
        }
    }
}

int Command_Line_Runner::Parse_Command() {
    if (this->args->size() <= 1) return true; //nothing to run
    QString commandString = this->args->at(1).toLower();
    this->Remove_Command_Identifier_Characters(commandString);

    QMap<QString, int>::iterator iter = this->commandMap->find(commandString);
    if (iter == this->commandMap->end()) return 0; //not found, so show help
    else return iter.value();
}

void Command_Line_Runner::Remove_Command_Identifier_Characters(QString &command) {
    if (command.startsWith("/")) {
        command = command.remove(0, 1);
        return;
    } else if (command.startsWith("--")) {
        command = command.remove(0, 2);
        return;
    } else if (command.startsWith("-")) {
        command = command.remove(0, 1);
        return;
    }
}

int Command_Line_Runner::Show_Error(int command, int errorCode) {
    if (errorCode != 0) {
        qInfo().nospace() << "The operation failed with error code " << errorCode << "!";
        qInfo() << "Try using the GUI for more details if the error persists.";
    } else {
        QString fileName = this->Get_Operated_File_Name();
        if (fileName.isEmpty()) {
            if (command == 1) qInfo() << "Packed successfully!";
            else if (command == 2) qInfo() << "Unpacked successfully!";
            else qInfo() << "Done!";
        } else {
            if (command == 1) qInfo().noquote() << "Packed" << fileName << "successfully!";
            else if (command == 2) qInfo().noquote() << "Unpacked" << fileName << "successfully!";
            else qInfo() << "Done!";
        }
    }
    return errorCode;
}

void Command_Line_Runner::Show_Help() {
    qInfo() << "";

    //Pack
    qInfo().noquote() << this->args->at(0) << "--pack [Source Folder]";
    qInfo().noquote() << this->args->at(0) << "--pack [Source Folder] [Destination Archive]";
    qInfo() << "    Packs a folder into an archive";
    qInfo() << "";

    //Unpack
    qInfo().noquote() << this->args->at(0) << "--unpack [Source Archive]";
    qInfo().noquote() << this->args->at(0) << "--unpack [Source Archive] [Destination Folder]";
    qInfo() << "    Unpacks an archive into a folder";
    qInfo() << "";

    //Help
    qInfo().noquote() << this->args->at(0) << "--help";
    qInfo() << "    Shows this message";
}

void Command_Line_Runner::Show_Invalid_Number_Of_Arguments_Message() {
    qInfo() << "Invalid number of arguments";
}

QString Command_Line_Runner::Get_Operated_File_Name() {
    QString filePath = QString();
    if (this->args->size() > 2) {
        filePath = this->args->at(3);
    } else if (this->args->size() == 2) {
        filePath = this->args->at(2);
    } else {
        return filePath;
    }
    QFileInfo fileInfo(filePath);
    return fileInfo.fileName();
}

