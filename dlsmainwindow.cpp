#include "dlsmainwindow.hpp"
#include "ui_dlsmainwindow.h"

DLSMainWindow::DLSMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DLSMainWindow)
{
    ui->setupUi(this);
}

DLSMainWindow::~DLSMainWindow()
{
    delete ui;
}
