#include "Viewer.h"
#include "ui_Viewer.h"
#include "../Sequential_Archive/Sequential_Archive_Interface.h"

Viewer::Viewer(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin) :
    QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::Viewer)
{
    assert(sequentialArchivePlugin);
    this->sequentialArchivePlugin = sequentialArchivePlugin;
    ui->setupUi(this);

    //Set up the splitter view
    this->splitter = new QSplitter(this);
    this->ui->horizontalLayout->addWidget(this->splitter);
    this->treeView = new QTreeView(this->splitter);
    this->splitter->addWidget(this->treeView);
    this->textBrowser = new QTextBrowser(this->splitter);
    this->splitter->addWidget(this->textBrowser);
}

Viewer::~Viewer() {
    delete ui;
}
