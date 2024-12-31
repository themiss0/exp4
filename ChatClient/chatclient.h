#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(QObject *parent = nullptr);
    ~ChatClient();

signals:
    void connected();
    void messageReceived(const QString &text);
    void jsonReceived(const QJsonObject &docObj);


private:
    QTcpSocket *m_clientSocket = nullptr;

public slots:
    void onReadyRead();
    void sendMessage(const QString &text, const QString &type = "message");
    void connectToServer(const QString &name, const QHostAddress &address, quint16 port);


};

#endif // CHATCLIENT_H
