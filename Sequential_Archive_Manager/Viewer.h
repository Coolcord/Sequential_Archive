#ifndef VIEWER_H
#define VIEWER_H

#include <QDialog>
#include <QSplitter>
#include <QTextBrowser>
#include <QTreeView>

class Sequential_Archive_Interface;

namespace Ui {
class Viewer;
}

class Viewer : public QDialog
{
    Q_OBJECT

public:
    Viewer(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin);
    ~Viewer();

private:
    Ui::Viewer *ui;
    QSplitter *splitter;
    QTextBrowser *textBrowser;
    QTreeView *treeView;
    Sequential_Archive_Interface *sequentialArchivePlugin;
};

#endif // VIEWER_H
