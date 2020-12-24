#include "webclient.h"

#include <QMetaObject>

WebClient::WebClient(QWebSocket *socket, QObject *parent)
    : QThread(parent)
    //m_socket(socket)
{
    m_socket = socket;
}

void WebClient::init()
{

}

QWebSocket *WebClient::socket() const
{
    return m_socket;
}


void WebClient::run()
{
    connect(m_socket, SIGNAL(textMessageReceived(const QString&)), this, SLOT(newMessageText(const QString&)));

    exec();
}

void WebClient::newMessageText(const QString & msg)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(msg.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    qDebug() << "Received:" << jsonObject;

    QMetaObject::invokeMethod(this, jsonObject["id"].toString().toStdString().data(), Qt::DirectConnection, Q_ARG(QJsonObject, jsonObject));

//    _connect_json(jsonObject, createUser);
//    _connect_json(jsonObject, removeUser);
//    _connect_json(jsonObject, updateUser);
//    _connect_json(jsonObject, sUpdateUser);
//    _connect_json(jsonObject, setImgUser);
//    _connect_json(jsonObject, getUserInfo);
//    _connect_json(jsonObject, isExist);
//    _connect_json(jsonObject, connectUser);
//    _connect_json(jsonObject, disconnectUser);
//    _connect_json(jsonObject, getChatsUser);
//    _connect_json(jsonObject, createChat);
//    _connect_json(jsonObject, removeChat);
//    _connect_json(jsonObject, updateChat);
//    _connect_json(jsonObject, setImgChat);
//    _connect_json(jsonObject, updateUserPosition);
//    _connect_json(jsonObject, addUserToChat);
//    _connect_json(jsonObject, removeUserFromChat);
//    _connect_json(jsonObject, getChatInfo);
//    _connect_json(jsonObject, getUsersChat);
//    _connect_json(jsonObject, getMsgsChat);
//    _connect_json(jsonObject, createMsg);
//    _connect_json(jsonObject, setFile);
//    _connect_json(jsonObject, removeMsg);
//    _connect_json(jsonObject, updateMsg);
//    _connect_json(jsonObject, readMsg);
//    _connect_json(jsonObject, addContact);
//    _connect_json(jsonObject, removeContact);
//    _connect_json(jsonObject, blockContact);
//    _connect_json(jsonObject, unblockContact);
//    _connect_json(jsonObject, findUsers);
//    _connect_json(jsonObject, findChats);
//    _connect_json(jsonObject, findMsgs);
}

void WebClient::pushMessage(const QJsonObject & msg)
{
    m_socket->sendTextMessage(QJsonDocument(msg).toJson());
}

void WebClient::incorrectRequest(const QString &id)
{
    QJsonObject json;
    json.insert("id", id);
    json.insert("error", REQUEST_ERROR_TEXT);
    sendJSON(json);
}

user_id WebClient::id() const
{
    return m_id;
}

void WebClient::createUser(const QJsonObject &request)
{    
    QString _get_json_value(login,      request, toString());
    QString _get_json_value(password,   request, toString());
    QString _get_json_value(name,       request, toString());
    QString _get_json_value(info,       request, toString());

    QJsonObject json = m_database->q2_create_user(login, password, name, info);
    if(json["error"] == "none")
        m_id = json["userid"].toInt();

    json.insert("id", request["id"]);
    sendJSON(json);
}

void WebClient::removeUser(const QJsonObject &request)
{
    QString _get_json_value(password,      request, toString());

    QJsonObject json = m_database->q2_remove_user(m_id, password);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "userRemoved");
        _json.insert("userid", m_id);
        emit toAll(m_id, _json);
    }
    json.insert("id", request["id"]);
    sendJSON(json);
}

void WebClient::updateUser(const QJsonObject &request)
{
    QString _get_json_value(param,      request, toString());
    QString _get_json_value(value,      request, toString());

    QJsonObject json = m_database->q2_update_user(m_id, param, value);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "userChanged");
        _json.insert("userid", m_id);
        _json.insert("param", param);
        _json.insert("value", value);
        emit toAll(m_id, _json);
    }
    json.insert("id", request["id"]);
    sendJSON(json);
}

void WebClient::sUpdateUser(const QJsonObject &request)
{
    QString _get_json_value(param,      request, toString());
    QString _get_json_value(value,      request, toString());
    QString _get_json_value(password,   request, toString());

    QJsonObject json = m_database->q2_supdate_user(m_id, param, value, password);
    json.insert("id", request["id"]);
    sendJSON(json);
}

void WebClient::setImgUser(const QJsonObject &request)
{
    QByteArray _get_json_value(img, request, toString().toUtf8());

    QFile file(QCoreApplication::applicationDirPath() + "/userImg/" + QString::number(m_id));
    file.open(QIODevice::WriteOnly);
    file.write(img);
    file.close();
}

void WebClient::getUserInfo(const QJsonObject &request)
{
    user_id _get_json_value(userid, request, toInt());

    QJsonObject json = m_database->q2_get_user(m_id, userid);
    json.insert("id", "getUserInfo");
    sendJSON(json);

    QFile file(QCoreApplication::applicationDirPath() + "/userImg/" + QString::number(m_id));
    if(file.exists()){
        file.open(QIODevice::ReadOnly);
        QJsonObject _json;
        _json.insert("id", "getUserInfo+");
        _json.insert("userid", userid);
        _json.insert("img", QString(file.readAll()));
        file.close();
        m_socket->sendTextMessage(QJsonDocument(_json).toJson());
    }
}

void WebClient::isExist(const QJsonObject& request)
{
    QString _get_json_value(login, request, toString());

    QJsonObject json = m_database->q2_is_exist_user(login);
    json.insert("id", "isExist");
    sendJSON(json);
}

void WebClient::connectUser(const QJsonObject& request)
{
    QString _get_json_value(login,      request, toString());
    QString _get_json_value(password,   request, toString());

    QJsonObject json = m_database->q2_connect_user(login, password);
    if(json["error"] == "none"){
        m_id = json["userid"].toInt();
        QJsonObject _json;
        _json.insert("id", "userOnlineChanged");
        _json.insert("value", true);
        emit toAll(m_id, _json);
    }
    json.insert("id", "connectUser");
    sendJSON(json);
}

void WebClient::disconnectUser(const QJsonObject& request)
{
    Q_UNUSED(request);

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QJsonObject json = m_database->q2_disconnect_user(m_id, date.toString("yyyy-MM-dd"), time.toString());

    QJsonObject _json;
    _json.insert("id", "userOnlineChanged");
    _json.insert("value", false);
    emit toAll(m_id, _json);
}

void WebClient::getChatsUser(const QJsonObject& request)
{
    int _get_json_value(start,      request, toInt());
    int _get_json_value(end,        request, toInt());

    QJsonObject json = m_database->q2_get_chats_user(m_id, start, end);
    json.insert("id", "getChatsUser");
    sendJSON(json);

    QVector<chat_id> chats = m_database->q2_user_chats(m_id, start, end);
    for(auto e: chats){
        QFile file;
        file.setFileName(QCoreApplication::applicationDirPath() + "/chatImg/" + QString::number(e));
        if(file.exists()){
            file.open(QIODevice::ReadOnly);
            QJsonObject _json;
            _json.insert("id", "getChatsUser+");
            _json.insert("chatid", e);
            _json.insert("img", QString(file.readAll()));
            file.close();
            m_socket->sendTextMessage(QJsonDocument(_json).toJson());
        }
    }
}

void WebClient::createChat(const QJsonObject& request)
{
    QString _get_json_value(name,      request, toString());
    QString _get_json_value(info,      request, toString());
    int     _get_json_value(type,      request, toInt());

    QJsonObject json = m_database->q2_create_chat(m_id, name, info, type);
    json.insert("id", "createChat");
    sendJSON(json);
}

void WebClient::removeChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,      request, toInt());

    QJsonObject _json;
    _json.insert("id", "chatRemoved");
    _json.insert("chatid", chatid);
    emit toChat(chatid, _json);

    QJsonObject json = m_database->q2_remove_chat(chatid);
    json.insert("id", "removeChat");
    sendJSON(json);
}

void WebClient::updateChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,      request, toInt());
    QString _get_json_value(param,       request, toString());
    QString _get_json_value(value,       request, toString());

    QJsonObject json = m_database->q2_update_chat(chatid, param, value);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "userChanged");
        _json.insert("chatid", chatid);
        _json.insert("param", param);
        _json.insert("value", value);
        emit toChat(chatid, _json);
    }
    json.insert("id", "updateChat");
    sendJSON(json);
}

void WebClient::setImgChat(const QJsonObject& request)
{
    chat_id    _get_json_value(chatid,   request, toInt());
    QByteArray _get_json_value(img,      request, toString().toUtf8());

    QFile file(QCoreApplication::applicationDirPath() + "/chatImg/" + QString::number(chatid));
    file.open(QIODevice::WriteOnly);
    file.write(img);
    file.close();
}

void WebClient::updateUserPosition(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    user_id _get_json_value(userid,   request, toInt());
    int     _get_json_value(value,    request, toInt());

    QJsonObject json = m_database->q2_update_user_chat(chatid, value, userid);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "userPositionChanged");
        _json.insert("chatid", chatid);
        _json.insert("userid", userid);
        _json.insert("value", value);
        emit toChat(chatid, _json);
    }
    json.insert("id", "updateUserPosition");
    sendJSON(json);

    QString text;
    if(value == 1) text = m_database->getName(m_id) + " set admin: " + m_database->getName(userid);
    else text = m_database->getName(m_id) + " remove admin: " + m_database->getName(userid);
    QJsonObject _json;
    _json.insert("id", "createMsg");
    _json.insert("chatid", chatid);
    _json.insert("text", text);
    _json.insert("answerid", -1);
    _json.insert("type", -1);
    createMsg(_json);
}

void WebClient::addUserToChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    user_id _get_json_value(userid,   request, toInt());
    int     _get_json_value(type,     request, toInt());

    QJsonObject json = m_database->q2_add_user_chat(chatid, userid, 0);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "userAddedToChat");
        _json.insert("chatid", chatid);
        _json.insert("userid", userid);
        emit toChat(chatid, _json);
    }
    json.insert("id", "addUserToChat");
    sendJSON(json);

    QString text;
    if(type == 0) text = m_database->getName(m_id) + " add " + m_database->getName(userid);
    else text = m_database->getName(userid) + " connected";
    QJsonObject _json;
    _json.insert("id", "createMsg");
    _json.insert("chatid", chatid);
    _json.insert("text", text);
    _json.insert("answerid", -1);
    _json.insert("type", -1);
    createMsg(_json);
}

void WebClient::removeUserFromChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    user_id _get_json_value(userid,   request, toInt());
    int     _get_json_value(type,     request, toInt());

    QJsonObject json = m_database->q2_remove_user_chat(chatid, userid);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "removeUserFromChat");
        _json.insert("chatid", chatid);
        _json.insert("userid", userid);
        emit toChat(chatid, _json);
    }
    json.insert("id", "removeUserFromChat");
    sendJSON(json);

    QString text;
    if(type == 0) text = m_database->getName(userid) + " removed " + m_database->getName(m_id);
    else text = m_database->getName(userid) + " disconnect";
    QJsonObject _json;
    _json.insert("id", "createMsg");
    _json.insert("chatid", chatid);
    _json.insert("text", text);
    _json.insert("answerid", -1);
    _json.insert("type", -1);
    createMsg(_json);
}

void WebClient::getChatInfo(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    user_id _get_json_value(userid,   request, toInt());

    QJsonObject json = m_database->q2_get_chat(userid, chatid);
    json.insert("id", "getChatInfo");
    sendJSON(json);
    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath() + "/chatImg/" + QString::number(chatid));
    if(file.exists()){
        file.open(QIODevice::ReadOnly);
        QJsonObject _json;
        _json.insert("id", "getChatInfo+");
        _json.insert("chatid", chatid);
        _json.insert("img", QString(file.readAll()));
        file.close();
        m_socket->sendTextMessage(QJsonDocument(_json).toJson());
    }
}

void WebClient::getUsersChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    int     _get_json_value(start,    request, toInt());
    int     _get_json_value(end,      request, toInt());

    QJsonObject json = m_database->q2_get_users_chat(chatid, start, end);
    json.insert("id", "getUsersChat");
    sendJSON(json);

    QVector<user_id> users = m_database->q2_chat_members(chatid, start, end);
    for(auto e: users){
        QFile file;
        file.setFileName(QCoreApplication::applicationDirPath() + "/userImg/" + QString::number(e));
        if(file.exists()){
            file.open(QIODevice::ReadOnly);
            QJsonObject _json;
            _json.insert("id", "getUsersChat+");
            _json.insert("userid", e);
            _json.insert("chatid", chatid);
            _json.insert("img", QString(file.readAll()));
            file.close();
            m_socket->sendTextMessage(QJsonDocument(_json).toJson());
        }
    }
}

void WebClient::getMsgsChat(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    int     _get_json_value(start,    request, toInt());
    int     _get_json_value(end,      request, toInt());

    QJsonObject json = m_database->q2_get_msgs_chat(chatid, start, end);
    json.insert("id", "getMsgsChat");
    sendJSON(json);

    for(int e = start; e < end; e++){
        QFile file;
        file.setFileName(QCoreApplication::applicationDirPath() + "/msgFiles/" + QString::number(chatid) + "_" + QString::number(e));
        if(file.exists()){
            file.open(QIODevice::ReadOnly);
            QJsonObject _json;
            _json.insert("id", "getMsgsChat+");
            _json.insert("msgid", e);
            _json.insert("chatid", chatid);
            _json.insert("file", QString(file.readAll()));
            file.close();
            m_socket->sendTextMessage(QJsonDocument(_json).toJson());
        }
    }
}

void WebClient::createMsg(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,   request, toInt());
    QString _get_json_value(text,     request, toString());
    int     _get_json_value(answerid, request, toInt());
    int     _get_json_value(type,     request, toInt());

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QJsonObject json = m_database->q2_create_msg(m_id, chatid, text, date.toString("yyyy-MM-dd"), time.toString(), answerid, type);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "newMsg");
        _json.insert("chatid", chatid);
        _json.insert("msg", m_database->q3_msg_json(chatid, json["msgid"].toInt()));
        emit toChat(chatid, _json);
    }
    json.insert("id", "createMsg");
    sendJSON(json);
}

void WebClient::setFile(const QJsonObject& request)
{
    chat_id    _get_json_value(chatid,  request, toInt());
    msg_id     _get_json_value(msgid,   request, toInt());
    QByteArray _get_json_value(data,    request, toString().toUtf8());

    QFile file(QCoreApplication::applicationDirPath() + "/msgFiles/" + QString::number(chatid) + "_" + QString::number(msgid));
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

void WebClient::removeMsg(const QJsonObject& request)
{
    chat_id    _get_json_value(chatid,  request, toInt());
    msg_id     _get_json_value(msgid,   request, toInt());

    QJsonObject json = m_database->q2_remove_msg(chatid, msgid);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "msgRemoved");
        _json.insert("chatid", chatid);
        _json.insert("msgid", msgid);
        emit toChat(chatid, _json);
    }
    json.insert("id", "removeMsg");
    sendJSON(json);
}

void WebClient::updateMsg(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,  request, toInt());
    msg_id  _get_json_value(msgid,   request, toInt());
    QString _get_json_value(text,    request, toString());

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QJsonObject json = m_database->q2_update_msg(chatid, msgid, text, date.toString("yyyy-MM-dd"), time.toString());
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "editMsg");
        _json.insert("chatid", chatid);
        _json.insert("msg", m_database->q3_msg_json(chatid, msgid));
        emit toChat(chatid, _json);
    }
    json.insert("id", "updateMsg");
    sendJSON(json);
}

void WebClient::readMsg(const QJsonObject& request)
{
    chat_id _get_json_value(chatid,  request, toInt());
    msg_id  _get_json_value(msgid,   request, toInt());

    QJsonObject json = m_database->q2_read_msg(m_id, chatid, msgid);
    json.insert("id", "readMsg");
    sendJSON(json);
}

void WebClient::addContact(const QJsonObject& request)
{
    user_id _get_json_value(userid,  request, toInt());

    QJsonObject json = m_database->q2_add_contact(m_id, userid);
    json.insert("id", "addContact");
    sendJSON(json);
}

void WebClient::removeContact(const QJsonObject& request)
{
    user_id _get_json_value(userid,  request, toInt());

    QJsonObject json = m_database->q2_remove_contact(m_id, userid);
    json.insert("id", "removeContact");
    sendJSON(json);
}

void WebClient::blockContact(const QJsonObject& request)
{
    user_id _get_json_value(userid,  request, toInt());

    QJsonObject json = m_database->q2_block_contact(m_id, userid, 1);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "contactBlocked");
        _json.insert("userid", m_id);
        emit toUser(userid, _json);
    }
    json.insert("id", "blockContact");
    sendJSON(json);
}

void WebClient::unblockContact(const QJsonObject& request)
{
    user_id _get_json_value(userid,  request, toInt());

    QJsonObject json = m_database->q2_block_contact(m_id, userid, 0);
    if(json["error"] == "none"){
        QJsonObject _json;
        _json.insert("id", "contactUnblocked");
        _json.insert("userid", m_id);
        emit toUser(userid, _json);
    }
    json.insert("id", "unblockContact");
    sendJSON(json);
}

void WebClient::findUsers(const QJsonObject& request)
{
    QString _get_json_value(text,   request, toString());
    int     _get_json_value(start,  request, toInt());
    int     _get_json_value(end,    request, toInt());

    QJsonObject json = m_database->q2_find_user(m_id, text, start, end);
    json.insert("id", "findUsers");
    sendJSON(json);
}

void WebClient::findChats(const QJsonObject& request)
{
    QString _get_json_value(text,   request, toString());
    int     _get_json_value(start,  request, toInt());
    int     _get_json_value(end,    request, toInt());

    QJsonObject json = m_database->q2_find_chat(text, start, end);
    json.insert("id", "findChats");
    sendJSON(json);
}

void WebClient::findMsgs(const QJsonObject& request)
{
    chat_id _get_json_value(chatid, request, toInt());
    QString _get_json_value(text,   request, toString());
    int     _get_json_value(start,  request, toInt());
    int     _get_json_value(end,    request, toInt());

    QJsonObject json = m_database->q2_find_msg(text, chatid, start, end);
    json.insert("id", "findMsgs");
    sendJSON(json);
}

void WebClient::reconnectUser(const QJsonObject &request)
{
    user_id _get_json_value(userid, request, toInt());

    m_id = userid;

    QJsonObject json;
    json.insert("id", request["id"]);
    json.insert("error", "none");
    sendJSON(json);
}

void WebClient::setDatabase(Database *database)
{
    m_database = database;
}

void WebClient::sendJSON(QJsonObject &json)
{
    QString text = QJsonDocument(json).toJson();
    qDebug() << QJsonDocument(json);
    m_socket->sendTextMessage(text);
}
