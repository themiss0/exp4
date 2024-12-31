#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include "serverworker.h"
#include <QJsonObject>
#include <QJsonValue>

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    void broadcast(const QJsonObject &message, ServerWorker *exclude);

signals:
    void logMessage(const QString &message);

public slots:
    void stopServer();
    void jsonReceived(ServerWorker *sender, const QJsonObject &docObj);


private:
    QVector<ServerWorker *>m_clients;

};

#endif // CHATSERVER_H
