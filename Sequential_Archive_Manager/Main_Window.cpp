#include "Main_Window.h"
#include "ui_Main_Window.h"
#include "Viewer.h"

Main_Window::Main_Window(QWidget *parent) :
    QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::Main_Window)
{
    ui->setupUi(this);
}

Main_Window::~Main_Window() {
    delete ui;
}

void Main_Window::on_btnPack_clicked() {

}

void Main_Window::on_btnUnpack_clicked() {

}

void Main_Window::on_btnView_clicked() {
    Viewer viewer(this);
    viewer.exec();
}
