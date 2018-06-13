#ifndef VIEWER_H
#define VIEWER_H

#include <QDialog>
#include <QSplitter>
#include <QTextBrowser>
#include <QTreeWidget>

class Sequential_Archive_Interface;

namespace Ui {
class Viewer;
}

class Viewer : public QDialog {
    Q_OBJECT

public:
    Viewer(QWidget *parent, Sequential_Archive_Interface *sequentialArchivePlugin, const QString &openedFileName);
    ~Viewer();
    bool Populate_Window();
    bool Populate_Folder(QTreeWidgetItem *parent);

private:
    Ui::Viewer *ui;
    QSplitter *splitter;
    QTextBrowser *textBrowser;
    QTreeWidget *treeWidget;
    Sequential_Archive_Interface *sequentialArchivePlugin;
};

#endif // VIEWER_H
