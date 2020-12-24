#include "webserver.h"

WebServer::WebServer(const QString &serverName, QWebSocketServer::SslMode secureMode, QObject *parent)
    : QObject(parent)
{
    m_webSocketServer = new QWebSocketServer(serverName, secureMode, parent);
    connect(m_webSocketServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

bool WebServer::start(int port)
{
    if(m_webSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server is launched on" << m_webSocketServer->serverUrl().toString() << "!";
        qDebug() << "Listening...";
        if(m_database->open()) qDebug() << "Database is open!";
        return true;
    }
    qDebug() <<  m_webSocketServer->errorString();
    qDebug() << "Server launch error!";
    return false;
}

void WebServer::stop()
{
    m_webSocketServer->close();
    m_database->close();
    qDebug() << "Server is stopped!";
}

void WebServer::newConnection()
{
    if(m_webSocketServer->hasPendingConnections()){
        QWebSocket* clientSocket;
        if((clientSocket = m_webSocketServer->nextPendingConnection()) == nullptr) return;
        WebClient* client = new WebClient(clientSocket, this);
        client->setDatabase(m_database);
        connect(client, SIGNAL(toAll(user_id, QJsonObject)), this, SLOT(toAll(user_id, QJsonObject)));
        connect(client, SIGNAL(toChat(chat_id, QJsonObject)), this, SLOT(toChat(chat_id, QJsonObject)));
        connect(client, SIGNAL(toUser(user_id, QJsonObject)), this, SLOT(toUser(user_id, QJsonObject)));
        connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));
        m_clients.push_back(client);
        client->start();
        qDebug() << "New connection: " << clientSocket->peerAddress();
    }
}

void WebServer::disconnected()
{
    QWebSocket* client = static_cast<QWebSocket*>(sender());
    for(int i = 0; i < m_clients.size(); i++)
        if(m_clients[i]->socket() == client) {
            qDebug() << "User" << m_clients[i]->id() << "disconnected";
            m_clients[i]->quit();
            m_clients.erase(m_clients.begin() + i);
            break;
        }
}

void WebServer::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    QWebSocket* client = static_cast<QWebSocket*>(sender());
    for(int i = 0; i < m_clients.size(); i++)
        if(m_clients[i]->socket() == client) {
            qDebug() << "User" << m_clients[i]->id() << "disconnected with error" << client->errorString();
            m_clients[i]->quit();
            m_clients.erase(m_clients.begin() + i);
            break;
        }
}

void WebServer::toAll(user_id userid, QJsonObject json)
{
    QVector<user_id> users = m_database->getAll(userid);
    for(auto e: users)
        for(auto e2: m_clients)
            if(e2->id() == e) e2->pushMessage(json);
}

void WebServer::toChat(chat_id chatid, QJsonObject json)
{
    QVector<user_id> users = m_database->q2_chat_members(chatid);
    for(auto e: users)
        for(auto e2: m_clients)
            if(e2->id() == e) e2->pushMessage(json);
}

void WebServer::toUser(user_id userid, QJsonObject json)
{
    for(auto e2: m_clients)
        if(e2->id() == userid) e2->pushMessage(json);
}

void WebServer::setDatabase(Database *database)
{
    m_database = database;
}
