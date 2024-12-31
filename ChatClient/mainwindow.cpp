#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include "chatclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_chatClient = new ChatClient();
    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sayButton_clicked()
{
    if(!ui->sayLineEdit->text().isEmpty()){
        m_chatClient->sendMessage(ui->sayLineEdit->text());
        ui->sayLineEdit->setText("");
    }
}


void MainWindow::on_logoutButton_clicked()
{
    m_chatClient->sendMessage(ui->nameEdit->text(), "logout");
    ui->root->setCurrentIndex(0);
}


void MainWindow::on_loginButton_clicked()
{
    m_chatClient->connectToServer(QHostAddress(ui->urlEdit->text()), 1967);
}

void MainWindow::connectedToServer()
{
    ui->root->setCurrentIndex(1);
}

void MainWindow::messageReceived(const QString &text)
{
    ui->roomTextEdit->append(text);
}
