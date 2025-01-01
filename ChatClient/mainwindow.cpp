#include "mainwindow.h"
#include "qjsonarray.h"
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
    delete m_chatClient;
    m_chatClient = nullptr;
}

//登录键槽函数
void MainWindow::on_loginButton_clicked()
{
    //如果client非空，则删除它
    if(m_chatClient != nullptr){
        delete m_chatClient;
    }
    //创建一个新的client
    m_chatClient = new ChatClient(this);
    //连接信号和槽
    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);
    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    //client连接服务器
    m_chatClient->connectToServer(ui->nameEdit->text(), QHostAddress(ui->urlEdit->text()), 1967);
}

//连接到服务器后的槽函数
void MainWindow::connectedToServer()
{
    //发送登录消息
    m_chatClient->sendMessage(ui->nameEdit->text(), "login");
    //跳转到主页面
    ui->root->setCurrentIndex(1);
}
//接收到消息的槽函数
void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->roomTextEdit->append(QString("%1 : %2").arg(sender).arg(text));
}
//接收到json的槽函数
void MainWindow::jsonReceived(const QJsonObject &docObj)
{
    //检查完整性
    const QJsonValue typeVal = docObj.value("type");
    if(typeVal.isNull() || !typeVal.isString())
        return;
    //如果类型为message
    if(typeVal.toString().compare("message", Qt::CaseInsensitive)==0){
        const QJsonValue textVal = docObj.value("text");
        const QJsonValue senderVal = docObj.value("sender");

        if(textVal.isNull() || !textVal.isString())
            return;

        if(senderVal.isNull() || !senderVal.isString())
            return;

        messageReceived(senderVal.toString(), textVal.toString());
    //类型为newuser
    }else if(typeVal.toString().compare("newuser", Qt::CaseInsensitive) == 0){
        const QJsonValue usernameVal = docObj.value("username");
        if(usernameVal.isNull() || !usernameVal.isString())
            return;

        userJoined(usernameVal.toString());
    //类型为userdisconnected
    }else if(typeVal.toString().compare("userdisconnected", Qt::CaseInsensitive) == 0){
        const QJsonValue usernameVal = docObj.value("username");
        if(usernameVal.isNull() || !usernameVal.isString())
            return;

        userLefted(usernameVal.toString());
    //类型为userlist
    }else if(typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0){
        const QJsonValue userlistVal = docObj.value("userlist");
        if(userlistVal.isNull() || !userlistVal.isArray())
            return;
        userListReceived(userlistVal.toArray());
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

void MainWindow::userListReceived(const QJsonArray &list)
{
    ui->userList->clear();
    for (const QJsonValue &value : list) {
        ui->userList->addItem(value.toString());
    }
}

