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
ChatClient::~ChatClient(){
    if(m_clientSocket != nullptr)
        m_clientSocket->close();
    delete m_clientSocket;
}
//接收消息
void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData;
        if(socketStream.commitTransaction()){

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            //如果接收到完整消息，则发送信号
            if(jsonDoc.isObject()){
                emit jsonReceived(jsonDoc.object());
            }
        }else{
            break;
        }
    }

}
//发送消息
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
//连接服务器
void ChatClient::connectToServer(const QString &name, const QHostAddress &address, quint16 port)
{
    try{
        m_clientSocket->connectToHost(address, port);
    }catch(std::exception &e){
        qDebug() << QString::fromUtf8(e.what());
    }
}
