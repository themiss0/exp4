#include "mainwindow.h"
#include "qjsonobject.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include "chatclient.h"
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect(m_chatClient, &ChatClient::messageReceived, this, &MainWindow::messageReceived);
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

    for(auto aItem : ui->userList->findItems(ui->nameEdit->text(), Qt::MatchExactly)){
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}


void MainWindow::on_loginButton_clicked()
{
    if(m_chatClient != nullptr){
        delete m_chatClient;
    }

    m_chatClient = new ChatClient(this);

    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);
    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    m_chatClient->connectToServer(ui->nameEdit->text(), QHostAddress(ui->urlEdit->text()), 1967);

}

void MainWindow::connectedToServer()
{
    ui->root->setCurrentIndex(1);
}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->roomTextEdit->append(QString("%1 : %2").arg(sender).arg(text));
}

void MainWindow::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");
    if(typeVal.isNull() || !typeVal.isString())
        return;
    if(typeVal.toString().compare("message", Qt::CaseInsensitive)==0){
        const QJsonValue textVal = docObj.value("text");
        const QJsonValue senderVal = docObj.value("sender");
        if(textVal.isNull() || !textVal.isString())
            return;

        if(senderVal.isNull() || !senderVal.isString())
            return;

        messageReceived(senderVal.toString(), textVal.toString());

    }else if(typeVal.toString().compare("newuser", Qt::CaseInsensitive) == 0){
        const QJsonValue usernameVal = docObj.value("username");
        if(usernameVal.isNull() || !usernameVal.isString())
            return;

        userJoined(usernameVal.toString());
    }else if(typeVal.toString().compare("userdisconnected", Qt::CaseInsensitive) == 0){
        const QJsonValue usernameVal = docObj.value("username");
        if(usernameVal.isNull() || !usernameVal.isString())
            return;

        userLefted(usernameVal.toString());
    }else if(typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0){
        const QJsonValue userlistVal = docObj.value("userlist");
        if(userlistVal.isNull() || !userlistVal.isArray())
            return;

        userListReceived(userlistVal.toVariant().toString());
    }
}

void MainWindow::userJoined(const QString &user)
{
    ui->userList->addItem(user);
}

void MainWindow::userLefted(const QString &user)
{
    for(auto aItem : ui->userList->findItems(user, Qt::MatchExactly)){
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::userListReceived(const QString &list)
{
    ui->userList->clear();
    ui->userList->addItem(list);
}

