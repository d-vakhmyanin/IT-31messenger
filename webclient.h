#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <QObject>
#include <QThread>
#include <QWebSocket>
#include <QFile>
#include <QCoreApplication>

#include "database.h"

#define REQUEST_ERROR_TEXT "Incorrect request."

#define _get_json_value(var, json, to) var; if(json[#var] == QJsonValue::Undefined) { incorrectRequest(request["id"].toString()); return; } var = json[#var].to;
#define _connect_json(json, function) if(json["id"].toString() == #function) { function(json); return; }
class WebClient : public QThread
{
    Q_OBJECT
public:
    explicit WebClient(QWebSocket* socket, QObject *parent = nullptr);
    static void init();

    QWebSocket *socket() const;
    void setDatabase(Database *database);

    void sendJSON(QJsonObject &json);

    user_id id() const;

    Q_INVOKABLE void createUser           (const QJsonObject& request);
    Q_INVOKABLE void removeUser           (const QJsonObject& request);
    Q_INVOKABLE void updateUser           (const QJsonObject& request);
    Q_INVOKABLE void sUpdateUser          (const QJsonObject& request);
    Q_INVOKABLE void setImgUser           (const QJsonObject& request);
    Q_INVOKABLE void getUserInfo          (const QJsonObject& request);
    Q_INVOKABLE void isExist              (const QJsonObject& request);
    Q_INVOKABLE void connectUser          (const QJsonObject& request);
    Q_INVOKABLE void disconnectUser       (const QJsonObject& request);
    Q_INVOKABLE void getChatsUser         (const QJsonObject& request);

    Q_INVOKABLE void createChat           (const QJsonObject& request);
    Q_INVOKABLE void removeChat           (const QJsonObject& request);
    Q_INVOKABLE void updateChat           (const QJsonObject& request);
    Q_INVOKABLE void setImgChat           (const QJsonObject& request);
    Q_INVOKABLE void updateUserPosition   (const QJsonObject& request);
    Q_INVOKABLE void addUserToChat        (const QJsonObject& request);
    Q_INVOKABLE void removeUserFromChat   (const QJsonObject& request);
    Q_INVOKABLE void getChatInfo          (const QJsonObject& request);
    Q_INVOKABLE void getUsersChat         (const QJsonObject& request);
    Q_INVOKABLE void getMsgsChat          (const QJsonObject& request);

    Q_INVOKABLE void createMsg            (const QJsonObject& request);
    Q_INVOKABLE void setFile              (const QJsonObject& request);
    Q_INVOKABLE void removeMsg            (const QJsonObject& request);
    Q_INVOKABLE void updateMsg            (const QJsonObject& request);
    Q_INVOKABLE void readMsg              (const QJsonObject& request);

    Q_INVOKABLE void addContact           (const QJsonObject& request);
    Q_INVOKABLE void removeContact        (const QJsonObject& request);
    Q_INVOKABLE void blockContact         (const QJsonObject& request);
    Q_INVOKABLE void unblockContact       (const QJsonObject& request);

    Q_INVOKABLE void findUsers            (const QJsonObject& request);
    Q_INVOKABLE void findChats            (const QJsonObject& request);
    Q_INVOKABLE void findMsgs             (const QJsonObject& request);

    Q_INVOKABLE void reconnectUser        (const QJsonObject& request);
signals:
    void newID(user_id userid);
    void toAll(user_id userid, QJsonObject json);
    void toChat(chat_id chatid, QJsonObject json);
    void toUser(user_id userid, QJsonObject json);
public slots:
    void newMessageText(const QString&);
    void pushMessage(const QJsonObject&);
private:
    void incorrectRequest(const QString& id);

    QWebSocket* m_socket = nullptr;
    Database* m_database = nullptr;
    user_id m_id = 1;
    
    // QThread interface
protected:
    void run() override;

    static QMap<QString, void (*) (const QJsonObject&)> functions;
};

#endif // WEBCLIENT_H
