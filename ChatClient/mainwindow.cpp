#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sayButton_clicked()
{

}


void MainWindow::on_logoutButton_clicked()
{
    ui->root->setCurrentIndex(0);
}


void MainWindow::on_loginButton_clicked()
{
    ui->root->setCurrentIndex(1);
}

