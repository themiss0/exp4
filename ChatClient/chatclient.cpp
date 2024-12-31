#include "chatclient.h"
#include <QJsonObject>
#include <QJsonDocument>


ChatClient::ChatClient(QObject *parent)
    : QObject{parent}
{
    m_clientSocket = new QTcpSocket(this);


    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData;
        if(socketStream.commitTransaction()){
            emit messageReceived(QString::fromUtf8((jsonData)));
            //在这里向界面打印输出
        }else{
            break;
        }
    }

}

void ChatClient::sendMessage(const QString &text, const QString &type)
{
    if(m_clientSocket->state() != QAbstractSocket::ConnectedState){
        return;
    }

    if(!text.isEmpty()){
        QDataStream serverStream(m_clientSocket);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        serverStream << QJsonDocument(message).toJson();

    }
}

void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    try{
        m_clientSocket->connectToHost(address, port);
    }catch(std::exception &e){
        qDebug() << QString::fromUtf8(e.what());
    }
}
