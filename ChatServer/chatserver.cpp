#include "chatserver.h"
#include "serverworker.h"
#include "serverworker.h"

ChatServer::ChatServer(QObject *parent):
    QTcpServer(parent)
{
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if(!worker->setSocketDescriptor(socketDescriptor)){
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);
    m_clients.append(worker);
    emit logMessage(("新的用户连接上了"));
}

void ChatServer::stopServer()
{
    close();
}
