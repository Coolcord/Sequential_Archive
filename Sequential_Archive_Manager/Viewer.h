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
    bool Populate_Folder(QTreeWidgetItem *parent, const QString &path);

private slots:
    void on_treeWidget_itemSelectionChanged();

private:
    Ui::Viewer *ui;
    Sequential_Archive_Interface *sequentialArchivePlugin;
};

#endif // VIEWER_H
