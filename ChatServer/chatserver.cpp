#include "chatserver.h"
#include "serverworker.h"
#include "serverworker.h"
#include <QJsonArray>

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent)
{
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    // 新建tcpworker
    ServerWorker *worker = new ServerWorker(this);
    if (!worker->setSocketDescriptor(socketDescriptor))
    {
        worker->deleteLater();
        return;
    }
    // 绑定
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);
    connect(worker, &ServerWorker::jsonReceived, this, &ChatServer::jsonReceived);
    connect(worker, &ServerWorker::disconnected, this, std::bind(&ChatServer::userDisconnected, this, worker));
    // 加入服务器列表
    m_clients.append(worker);
    emit logMessage(("新的用户连接上了"));
}
// 广播
void ChatServer::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    // 向列表所有用户发送信息
    for (ServerWorker *worker : m_clients)
    {
        if (worker != exclude)
        {
            worker->sendJson(message);
        }
    }
}

void ChatServer::stopServer()
{
    close();
}

void ChatServer::jsonReceived(ServerWorker *sender, const QJsonObject &docObj)
{
    // 检查json完整性
    const QJsonValue typeVal = docObj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
        return;
    // message类型
    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0)
    {
        const QJsonValue textVal = docObj.value("text");
        if (textVal.isNull() || !textVal.isString())
            return;
        const QString text = textVal.toString().trimmed();
        if (text.isEmpty())
            return;
        QJsonObject message;
        message["type"] = "message";
        message["text"] = text;
        message["sender"] = sender->getUserName();
        broadcast(message, sender);
        // login类型
    }
    else if (typeVal.toString().compare("login", Qt::CaseInsensitive) == 0)
    {
        const QJsonValue usernameVal = docObj.value("text");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        sender->setUserName(usernameVal.toString());
        // 向其他用户广播新用户
        QJsonObject connectedMessage;
        connectedMessage["type"] = "newuser";
        connectedMessage["username"] = usernameVal.toString();
        broadcast(connectedMessage, sender);
        // 产生用户列表
        QJsonObject userListMessage;
        userListMessage["type"] = "userlist";
        QJsonArray userlist;
        for (ServerWorker *worker : m_clients)
        {
            if (worker == sender)
                userlist.append(worker->getUserName() + "*");
            else
                userlist.append(worker->getUserName());
        }
        userListMessage["userlist"] = userlist;
        sender->sendJson(userListMessage);
    }
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->getUserName();

    if (!userName.isEmpty())
    {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = "userdisconnected";
        disconnectedMessage["username"] = userName;
        broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + "disconnected");
    }
    sender->deleteLater();
}
