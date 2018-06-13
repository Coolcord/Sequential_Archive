#include "Viewer.h"
#include "ui_Viewer.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include <assert.h>
#include <QTreeWidgetItem>

Viewer::Viewer(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin, const QString &openedFileName) :
    QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::Viewer)
{
    assert(sequentialArchivePlugin);
    this->sequentialArchivePlugin = sequentialArchivePlugin;
    ui->setupUi(this);

    //Set up the splitter view
    this->splitter = new QSplitter(this);
    this->ui->horizontalLayout->addWidget(this->splitter);
    this->treeWidget = new QTreeWidget(this->splitter);
    this->treeWidget->setColumnCount(1);
    this->treeWidget->setHeaderLabel(openedFileName);
    this->splitter->addWidget(this->treeWidget);
    this->textBrowser = new QTextBrowser(this->splitter);
    this->splitter->addWidget(this->textBrowser);
}

Viewer::~Viewer() {
    delete this->treeWidget; //automatically deallocates all QTreeWidgetItems
    delete this->textBrowser;
    delete this->splitter;
    delete ui;
}

bool Viewer::Populate_Window() {
    //QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this->treeWidget);
    //this->treeWidget->addTopLevelItem(topLevelItem);
    //topLevelItem->setText(0,"Item");
    //QTreeWidgetItem *item=new QTreeWidgetItem(topLevelItem);
    //item->setText(0,"SubItem");

    //Add the folders first
    if (!this->sequentialArchivePlugin->Change_Directory("/")) return false;
    QStringList folders = this->sequentialArchivePlugin->Get_Directories();
    for (int i = 0; i < folders.size(); ++i) {
        QTreeWidgetItem *folderItem = new QTreeWidgetItem(this->treeWidget);
        folderItem->setText(0, folders.at(i));
        this->treeWidget->addTopLevelItem(folderItem);
        if (!this->sequentialArchivePlugin->Change_Directory(folders.at(i))) return false;
        if (!this->Populate_Folder(folderItem)) return false;
    }

    //Add the files
    QStringList files = this->sequentialArchivePlugin->Get_Files();
    for (int i = 0; i < files.size(); ++i) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(this->treeWidget);
        fileItem->setText(0, files.at(i));
        this->treeWidget->addTopLevelItem(fileItem);
    }

    return true;
}

bool Viewer::Populate_Folder(QTreeWidgetItem *parent) {
    assert(parent);
    //Add the folders first
    QStringList folders = this->sequentialArchivePlugin->Get_Directories();
    for (int i = 0; i < folders.size(); ++i) {
        QTreeWidgetItem *folderItem = new QTreeWidgetItem(parent);
        folderItem->setText(0, folders.at(i));
        if (!this->sequentialArchivePlugin->Change_Directory(folders.at(i))) return false;
        if (!this->Populate_Folder(folderItem)) return false;
    }

    //Add the files
    QStringList files = this->sequentialArchivePlugin->Get_Files();
    for (int i = 0; i < files.size(); ++i) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);
        fileItem->setText(0, files.at(i));
    }

    return this->sequentialArchivePlugin->Change_Directory("..");
}
