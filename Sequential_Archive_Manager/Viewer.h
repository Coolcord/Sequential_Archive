#ifndef VIEWER_H
#define VIEWER_H

#include <QDialog>
#include <QSplitter>
#include <QTextBrowser>
#include <QTreeView>

namespace Ui {
class Viewer;
}

class Viewer : public QDialog
{
    Q_OBJECT

public:
    Viewer(QWidget *parent);
    ~Viewer();

private:
    Ui::Viewer *ui;
    QSplitter *splitter;
    QTextBrowser *textBrowser;
    QTreeView *treeView;
};

#endif // VIEWER_H
