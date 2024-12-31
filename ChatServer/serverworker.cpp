#include "serverworker.h"
#include <QJsonObject>
#include <QJsonDocument>

ServerWorker::ServerWorker(QObject *parent)
    : QObject{parent}
{
    m_serverSocket = new QTcpSocket(this);
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead);


}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData;
        if(socketStream.commitTransaction()){
            emit logMessage(QString::fromUtf8((jsonData)));
            sendMessage("I recieved message");
        }else{
            break;
        }
    }
}

void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if(m_serverSocket->state() != QAbstractSocket::ConnectedState){
        return;
    }

    if(!text.isEmpty()){
        QDataStream serverStream(m_serverSocket);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        serverStream << QJsonDocument(message).toJson();

    }
}

