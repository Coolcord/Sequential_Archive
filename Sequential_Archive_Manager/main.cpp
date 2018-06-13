#include "Main_Window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Main_Window w(NULL);
    w.show();

    return a.exec();
}
