#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QUrl>

#include <QtNetwork/QNetworkAccessManager>

#include "database.h"
#include "webclient.h"

class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(const QString &serverName, QWebSocketServer::SslMode secureMode, QObject *parent = nullptr);
    bool start(int port);
    void stop();
    void setDatabase(Database *database);

public slots:
    void newConnection();
    void disconnected();
    void error(QAbstractSocket::SocketError error);
    void toAll(user_id userid, QJsonObject json);
    void toChat(chat_id chatid, QJsonObject json);
    void toUser(user_id userid, QJsonObject json);
private:
    QWebSocketServer* m_webSocketServer = nullptr;
    Database* m_database = nullptr;
    QList<WebClient*> m_clients;
};

#endif // WEBSERVER_H
