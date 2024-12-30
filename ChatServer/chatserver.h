#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include "serverworker.h"

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void logMessage(const QString &message);

public slots:
    void stopServer();



private:
    QVector<ServerWorker *>m_clients;

};

#endif // CHATSERVER_H
