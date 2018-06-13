#include "Viewer.h"
#include "ui_Viewer.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"
#include <assert.h>
#include <QTreeWidgetItem>

#include <QDebug>

Viewer::Viewer(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin, const QString &openedFileName) :
    QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::Viewer)
{
    assert(sequentialArchivePlugin);
    this->sequentialArchivePlugin = sequentialArchivePlugin;
    ui->setupUi(this);

    //Set up the tree widget
    this->splitter = new QSplitter(this);
    this->ui->horizontalLayout->addWidget(this->splitter);
    this->treeWidget = new QTreeWidget(this->splitter);
    this->treeWidget->setColumnCount(2);
    this->treeWidget->setHeaderLabel(openedFileName);
    this->treeWidget->hideColumn(1);
    connect(this->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_treeWidget_itemSelectionChanged()));
    this->splitter->addWidget(this->treeWidget);

    //Set up the text browser
    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setPixelSize(20);
    this->textBrowser = new QTextBrowser(this->splitter);
    this->textBrowser->setFont(font);
    this->splitter->addWidget(this->textBrowser);
}

Viewer::~Viewer() {
    delete this->treeWidget; //automatically deallocates all QTreeWidgetItems
    delete this->textBrowser;
    delete this->splitter;
    delete ui;
}

bool Viewer::Populate_Window() {
    //Add the folders first
    if (!this->sequentialArchivePlugin->Change_Directory("/")) return false;
    QStringList folders = this->sequentialArchivePlugin->Get_Directories();
    for (int i = 0; i < folders.size(); ++i) {
        QTreeWidgetItem *folderItem = new QTreeWidgetItem(this->treeWidget);
        QString folderName = folders.at(i);
        folderItem->setText(0, folderName);
        folderItem->setText(1, "/"+folderName);
        this->treeWidget->addTopLevelItem(folderItem);
        if (!this->sequentialArchivePlugin->Change_Directory(folderName)) return false;
        if (!this->Populate_Folder(folderItem, "/"+folderName)) return false;
    }

    //Add the files
    QStringList files = this->sequentialArchivePlugin->Get_Files();
    for (int i = 0; i < files.size(); ++i) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(this->treeWidget);
        QString fileName = files.at(i);
        fileItem->setText(0, fileName);
        fileItem->setText(1, "/"+fileName);
        this->treeWidget->addTopLevelItem(fileItem);
    }

    return true;
}

bool Viewer::Populate_Folder(QTreeWidgetItem *parent, const QString &path) {
    assert(parent);
    //Add the folders first
    QStringList folders = this->sequentialArchivePlugin->Get_Directories();
    for (int i = 0; i < folders.size(); ++i) {
        QTreeWidgetItem *folderItem = new QTreeWidgetItem(parent);
        QString folderName = folders.at(i);
        folderItem->setText(0, folderName);
        folderItem->setText(1, path+"/"+folderName);
        if (!this->sequentialArchivePlugin->Change_Directory(folderName)) return false;
        if (!this->Populate_Folder(folderItem, path+"/"+folderName)) return false;
    }

    //Add the files
    QStringList files = this->sequentialArchivePlugin->Get_Files();
    for (int i = 0; i < files.size(); ++i) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);
        QString fileName = files.at(i);
        fileItem->setText(0, fileName);
        fileItem->setText(1, path+"/"+fileName);
    }

    return this->sequentialArchivePlugin->Change_Directory("..");
}

void Viewer::on_treeWidget_itemSelectionChanged() {
    QTreeWidgetItem *item = this->treeWidget->selectedItems().first();
    if (!item) return;
    this->textBrowser->setText(QString(this->sequentialArchivePlugin->Read_File(item->text(1))));
}
