#include "database.h"

Database::Database(QObject *parent) :
    QObject(parent)
{

}

Database::Database(QString path, QObject *parent) :
    QObject(parent)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(path);
}

//void Database::create(QString host, int port, QString userName, QString password)
//{
//    m_database = QSqlDatabase::addDatabase("QSQLITE");
//    m_database.setHostName(host);                       // "127.0.0.1"
//    m_database.setPort(port);                           // 3306
//    m_database.setUserName(userName);                   // "root"
//    m_database.setPassword(password);                   // "policeman"
//    m_database.setDatabaseName("C:/Users/Admin/Documents/build-WebServer_0_2-Desktop_Qt_5_15_1_MSVC2019_64bit-Release/real/dbmsg.db");
//}

bool Database::error() const
{
    return m_error;
}



QJsonObject Database::q2_create_user(const QString &login, const QString &password, const QString &name, const QString &info)
{
    QJsonObject result = q2_is_exist_user(login);
    if(result["error"].toString() != "none") return result;
    _err_json(result["result"].toBool() == true, "User already exist.");

    QSqlQuery query(m_database);
    QString queryText = QString("INSERT INTO %1 (%2, %3, %4, %5, %6) "
                                "VALUES (:value_1, :value_2, :value_3, :value_4, :value_5);")
            .arg(DB_USER_TABLE)
            .arg(DB_USER_LOGIN)
            .arg(DB_USER_PASSWORD)
            .arg(DB_USER_NAME)
            .arg(DB_USER_INFO)
            .arg(DB_USER_ONLINE);

    query.prepare(queryText);
    query.bindValue(":value_1", login);
    query.bindValue(":value_2", password);
    query.bindValue(":value_3", name);
    query.bindValue(":value_4", info);
    query.bindValue(":value_5", ONLINE);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    QString error;
    user_id userid = q2_autoincrement(DB_USER_TABLE, &error);
    _err_json(userid < 0, error);

    QJsonObject json;
    json.insert("userid", userid);
    _return_json(json);
}

QJsonObject Database::q2_remove_user(const user_id userid, const QString &password)
{
    QSqlQuery query(m_database);
    QString queryText = QString("DELETE FROM %1 "
                                "WHERE %2 = :value_1 AND %3 = :value_2;")
            .arg(DB_USER_TABLE)
            .arg(DB_USER_USERID)
            .arg(DB_USER_PASSWORD);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", password);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_update_user(const user_id userid, const QString &param, const QString &value)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2;")
            .arg(DB_USER_TABLE)
            .arg(param)
            .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1", value);
    query.bindValue(":value_2", userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_supdate_user(const user_id userid, const QString &param, const QString &value, const QString &password)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2 AND %4 = :value_3;")
            .arg(DB_USER_TABLE)
            .arg(param)
            .arg(DB_USER_USERID)
            .arg(DB_USER_PASSWORD);

    query.prepare(queryText);
    query.bindValue(":value_1", value);
    query.bindValue(":value_2", userid);
    query.bindValue(":value_3", password);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_get_user(const user_id userid, const user_id _userid)
{
    QSqlQuery query(m_database);
    QString queryText;
    queryText = QString("SELECT %1, %2, %3, %4, %5 "
                                "FROM %7 "
                                "WHERE %9 = :value_1;")
            .arg(DB_USER_NAME)
            .arg(DB_USER_INFO)
            .arg(DB_USER_DATE)
            .arg(DB_USER_TIME)
            .arg(DB_USER_ONLINE)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1",  _userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject contactJSON = q2_get_contact(userid, _userid);

    QJsonObject json;
    json.insert("id",        _userid);
    json.insert("name",      query.value(0).toString());
    json.insert("info",      query.value(1).toString());
    json.insert("contact",   contactJSON["status"].toInt());
    json.insert("online",    query.value(4).toInt());
    json.insert("date-time", query.value(2).toString() + " " + query.value(3).toString());
    _return_json(json);
}

QJsonObject Database::q2_is_exist_user(const QString &login)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT COUNT(*) "
                                "FROM %1 "
                                "WHERE %2 = :value_1;")
            .arg(DB_USER_TABLE)
            .arg(DB_USER_LOGIN);

    query.prepare(queryText);
    query.bindValue(":value_1", login);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("result", query.value(0).toInt() != 0);
    _return_json(json);
}

QJsonObject Database::q2_connect_user(const QString &login, const QString &password)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 AND %4 = :value_2;")
            .arg(DB_USER_USERID)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_LOGIN)
            .arg(DB_USER_PASSWORD);

    query.prepare(queryText);
    query.bindValue(":value_1", login);
    query.bindValue(":value_2", password);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    setOnline(query.value(0).toInt());

    QJsonObject json;
    json.insert("userid", query.value(0).toInt());
    _return_json(json);
}

QJsonObject Database::q2_disconnect_user(const user_id userid, const QString &date, const QString &time)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1, %3 = :value_2, %4 = :value_3 "
                                "WHERE %5 = :value_4;")
            .arg(DB_USER_TABLE)
            .arg(DB_USER_DATE)
            .arg(DB_USER_TIME)
            .arg(DB_USER_ONLINE)
            .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1", date);
    query.bindValue(":value_2", time);
    query.bindValue(":value_3", OFFLINE);
    query.bindValue(":value_4", userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_get_chats_user(const user_id userid, int start, int end)
{
    QSqlQuery query(m_database);

    QVector<user_id> chats = q2_user_chats(userid, start, end);

    QJsonArray jsonArr;
    for(auto e: chats){
        jsonArr.append(q2_json_chat(userid, e));
    }
    
    QJsonObject json;
    json.insert("arr", jsonArr);
    json.insert("start", start);
    json.insert("end", end);

    _return_json(json);
}

QJsonObject Database::q2_reconnect_user(const user_id userid)
{
    setOnline(userid);
    _return_true_json();
}

QJsonObject Database::q2_create_chat(const user_id userid, const QString &name, const QString &info, int type)
{
    QSqlQuery query(m_database);
    QString queryText = QString("INSERT INTO %1 (%2, %3, %4) "
                                "VALUES (:value_1, :value_2, :value_3);")
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_NAME)
            .arg(DB_CHAT_INFO)
            .arg(DB_CHAT_TYPE);

    query.prepare(queryText);
    query.bindValue(":value_1", name);
    query.bindValue(":value_2", info);
    query.bindValue(":value_3", type);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);
    
    QString error;
    user_id chatid = q2_autoincrement(DB_CHAT_TABLE, &error);
    _err_json(userid < 0, error);

    q2_add_user_chat(chatid, userid, 2);

    QJsonObject json;
    json.insert("chatid", chatid);
    _return_json(json);
}

QJsonObject Database::q2_remove_chat(/*const user_id userid,*/const chat_id chatid)
{
    QSqlQuery query(m_database);
    QVector<user_id> members = q2_chat_members(chatid);
    for(auto e: members){
        q2_remove_user_chat(chatid, e);
    }

    QString queryText = QString("DELETE FROM %1 "
                                "WHERE %2 = :value_1;")
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_update_chat(const chat_id chatid, const QString &param, const QString &value)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2")
            .arg(DB_CHAT_TABLE)
            .arg(param)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", value);
    query.bindValue(":value_2", chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_update_user_chat(const chat_id chatid, const int value, const user_id userid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2 AND %4 = :value_3")
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_POSITION)
            .arg(DB_USER_CHAT_CHATID)
            .arg(DB_USER_CHAT_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1", value);
    query.bindValue(":value_2", chatid);
    query.bindValue(":value_3", userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_add_user_chat(const chat_id chatid, const user_id userid, int position)
{
    QSqlQuery query(m_database);
    QString queryText = QString("INSERT INTO %1 (%2, %3, %4) "
                                "VALUES (:value_1, :value_2, :value_3);")
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID)
            .arg(DB_USER_CHAT_POSITION);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", chatid);
    query.bindValue(":value_3", position);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_remove_user_chat(const chat_id chatid, const user_id userid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("DELETE FROM %1 "
                                "WHERE %2 = :value_1 AND %3 = :value_2;")
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_get_chat(const user_id userid, const chat_id chatid)
{
    QString error;
    user_id members = q2_get_members(chatid, &error);
    _err_json(members < 0, error);

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2, %3 "
                                "FROM %4, %5 "
                                "WHERE %6 = :value_1 AND %7 = :value_2 AND %8 = :value_3;")
            .arg(DB_CHAT_NAME)
            .arg(DB_CHAT_INFO)
            .arg(DB_USER_CHAT_POSITION)
            .arg(DB_CHAT_TABLE)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_CHAT_CHATID)
            .arg(DB_USER_CHAT_CHATID)
            .arg(DB_USER_CHAT_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1",  chatid);
    query.bindValue(":value_2",  chatid);
    query.bindValue(":value_3",  userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("chatid",  chatid);
    json.insert("name",    query.value(0).toString());
    json.insert("info",    query.value(1).toString());
    json.insert("members", members);
    json.insert("admin",   query.value(2).toInt());
    _return_json(json);
}

QJsonObject Database::q2_get_users_chat(const chat_id chatid, int start, int end)
{
    QJsonArray result;
    QSqlQuery query(m_database);

    QVector<user_id> members = q2_chat_members(chatid, start, end);
    for(auto e: members){
        result.append(q2_json_user(e, chatid));
    }

    QJsonObject json;
    json.insert("chatid", chatid);
    json.insert("arr", result);
    json.insert("start", start);
    json.insert("end", end);
    _return_json(json);
}

QJsonObject Database::q2_get_msgs_chat(const chat_id chatid, int start, int end)
{
    QJsonArray result;
    QSqlQuery query(m_database);

    QVector<msg_id> msgs = q2_chat_msgs(chatid, start, end);
    for(auto e: msgs){
        result.append(q3_msg_json(chatid, e));
    }
    
    QJsonObject json;
    json.insert("chatid", chatid);
    json.insert("arr", result);
    json.insert("start", start);
    json.insert("end", end);
    _return_json(json);
}

QJsonObject Database::q2_create_msg(const user_id userid, const chat_id chatid, const QString &text, const QString &date, const QString &time, const msg_id answer, const int type)
{
    QString error;
    msg_id msgid = q2_get_chat_counter(chatid, &error);
    _err_json(msgid < 0, error);

    QSqlQuery query(m_database);
    QString queryText = QString("INSERT INTO %1 (%2, %3, %4, %5, %6, %7, %8, %9, %10) "
                                "VALUES (:value_0, :value_1, :value_2, :value_3, :value_4, :value_5, :value_6, :value_7, :value_8);")
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_MESSAGE_MSGID)
            .arg(DB_MESSAGE_USERID)
            .arg(DB_MESSAGE_CHATID)
            .arg(DB_MESSAGE_TEXT)
            .arg(DB_MESSAGE_DATE)
            .arg(DB_MESSAGE_TIME)
            .arg(DB_MESSAGE_ANSWER)
            .arg(DB_MESSAGE_TYPE)
            .arg(DB_MESSAGE_STATUS);

    query.prepare(queryText);
    query.bindValue(":value_0", msgid);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", chatid);
    query.bindValue(":value_3", text);
    query.bindValue(":value_4", date);
    query.bindValue(":value_5", time);
    query.bindValue(":value_6", answer);
    query.bindValue(":value_7", type);
    query.bindValue(":value_8", 1);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2;")
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_COUNTER)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", msgid + 1);
    query.bindValue(":value_2", chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    QJsonObject json;
    json.insert("msgid", msgid);
    _return_json(json);
}

QJsonObject Database::q2_remove_msg(const chat_id chatid, const msg_id msgid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("DELETE FROM %1 "
                                "WHERE %2 = :value_1 AND %3 = :value_2;")
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_MESSAGE_CHATID)
            .arg(DB_MESSAGE_MSGID);

    query.prepare(queryText);
    query.bindValue(":value_1", chatid);
    query.bindValue(":value_2", msgid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_update_msg(const chat_id chatid, const msg_id msgid, const QString &text, const QString &date, const QString &time)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1, %3 = :value_2, %4 = :value_3 "
                                "WHERE %5 = :value_4 AND %6 = :value_5;")
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_MESSAGE_TEXT)
            .arg(DB_MESSAGE_DATE)
            .arg(DB_MESSAGE_TIME)
            .arg(DB_MESSAGE_CHATID)
            .arg(DB_MESSAGE_MSGID);

    query.prepare(queryText);
    query.bindValue(":value_1", text);
    query.bindValue(":value_2", date);
    query.bindValue(":value_3", time);
    query.bindValue(":value_4", chatid);
    query.bindValue(":value_5", msgid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_read_msg(const user_id userid, const chat_id chatid, const msg_id msgid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2 AND %3 = :value_3;")
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_READ)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", msgid);
    query.bindValue(":value_2", userid);
    query.bindValue(":value_3", chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_add_contact(const user_id userid, const user_id contactid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("INSERT INTO %1 (%2, %3, %4) "
                                "VALUES (:value_1, :value_2, :value_3);")
            .arg(DB_CONTACT_TABLE)
            .arg(DB_CONTACT_USERID_1)
            .arg(DB_CONTACT_USERID_2)
            .arg(DB_CONTACT_STATUS);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", contactid);
    query.bindValue(":value_3", 0);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_remove_contact(const user_id userid, const user_id contactid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("DELETE FROM %1 "
                                "WHERE %2 = :value_1 AND %3 = :value_2;")
            .arg(DB_CONTACT_TABLE)
            .arg(DB_CONTACT_USERID_1)
            .arg(DB_CONTACT_USERID_2);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", contactid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_block_contact(const user_id userid, const user_id contactid, const int state)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2 AND %4 = :value_3;")
            .arg(DB_CONTACT_TABLE)
            .arg(DB_CONTACT_STATUS)
            .arg(DB_CONTACT_USERID_1)
            .arg(DB_CONTACT_USERID_2);

    query.prepare(queryText);
    query.bindValue(":value_1", state);
    query.bindValue(":value_2", userid);
    query.bindValue(":value_3", contactid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.numRowsAffected() == 0, AFFECTED_ROWS_ERROR);

    _return_true_json();
}

QJsonObject Database::q2_get_contact(const user_id userid, const user_id contactid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 AND %4 = :value_2;")
            .arg(DB_CONTACT_STATUS)
            .arg(DB_CONTACT_TABLE)
            .arg(DB_CONTACT_USERID_1)
            .arg(DB_CONTACT_USERID_2);

    query.prepare(queryText);
    query.bindValue(":value_1",  userid);
    query.bindValue(":value_2",  contactid);

    _err_json(!query.exec(), query.lastError().text());

    QJsonObject json;
    if(query.next() == 0) json.insert("status",  0);
    else json.insert("status",  query.value(0).toInt());

    _return_json(json);
}

QJsonObject Database::q2_find_user(const user_id userid, const QString &text, int start, int end)
{
    QJsonArray result;
    QSqlQuery query(m_database);

    QVector<user_id> users = q2_users(text, start, end);
    for(auto e: users){
        if(userid == e) continue;
        result.append(q2_json_user(e));
    }

    QJsonObject json;
    json.insert("arr", result);
    json.insert("start", start);
    json.insert("end", end);
    _return_json(json);
}

QJsonObject Database::q2_find_chat(const QString &text, int start, int end)
{
    QJsonArray result;
    QSqlQuery query(m_database);

    QVector<chat_id> chats = q2_chats(text, start, end);
    for(auto e: chats){
        result.append(q2_json_chat(e));
    }

    QJsonObject json;
    json.insert("arr", result);
    json.insert("start", start);
    json.insert("end", end);
    _return_json(json);
}

QJsonObject Database::q2_find_msg(const QString &text, const chat_id chatid, int start, int end)
{
    QJsonArray result;
    QSqlQuery query(m_database);

    QVector<msg_id> msgs = q2_msgs(text, start, end);
    for(auto e: msgs){
        result.append(q3_msg_json(chatid, e));
    }

    QJsonObject json;
    json.insert("chatid", chatid);
    json.insert("arr", result);
    json.insert("start", start);
    json.insert("end", end);
    _return_json(json);
}

int Database::q2_autoincrement(const QString table, QString *error)
{
    QSqlQuery query(m_database);
    QString id = "";
    if(table == DB_USER_TABLE) id = DB_USER_USERID;
    else id = DB_CHAT_CHATID;
    QString queryText = QString("SELECT MAX(%1) "
                                "FROM %2")
            .arg(id)
            .arg(table);
    query.prepare(queryText);

    if(!query.exec()){
        *error = query.lastError().text();
        return -1;
    }
    if(!query.next()){
        *error = NOTHING_FOUND;
        return -1;
    }

    return query.value(0).toInt();
//    QSqlQuery query(m_database);
//    QString queryText = QString("SELECT AUTO_INCREMENT "
//                                "FROM INFORMATION_SCHEMA.TABLES "
//                                "WHERE TABLE_SCHEMA = :value_1 AND TABLE_NAME = :value_2;");
//    query.prepare(queryText);
//    query.bindValue(":value_1", DB_SCHEMA);
//    query.bindValue(":value_2", table);

//    if(!query.exec()){
//        *error = query.lastError().text();
//        return -1;
//    }
//    if(!query.next()){
//        *error = NOTHING_FOUND;
//        return -1;
//    }

//    return query.value(0).toInt();
}

int Database::q2_user_position(const user_id userid, const chat_id chatid, QString *error)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 AND %4 = :value_2;")
            .arg(DB_USER_CHAT_POSITION)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", userid);
    query.bindValue(":value_2", chatid);

    if(!query.exec()){ 
        *error = query.lastError().text();
        return -1;
    }
    if(!query.next()){
        *error = NOTHING_FOUND;
        return -1;
    }

    return query.value(0).toInt();
}

int Database::q2_get_chat_counter(const chat_id chatid, QString *error)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1;")
            .arg(DB_CHAT_COUNTER)
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1", chatid);

    if(!query.exec()) {
        *error = query.lastError().text();
        return -1;
    }
    if(!query.next()){
        *error = NOTHING_FOUND;
        return -1;
    }

    return query.value(0).toInt();
}

int Database::q2_get_members(const chat_id chatid, QString *error)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT COUNT(*) "
                                "FROM %1 "
                                "WHERE %2 = :value_1")
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);

    if(!query.exec()){
        *error = query.lastError().text();
        return -1;
    }
    if(query.next() == 0){
        *error = NOTHING_FOUND;
        return -1;
    }

    return query.value(0).toInt();
}

QVector<user_id> Database::q2_chat_members(const chat_id chatid)
{
    QVector<user_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1;")
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);

    if(!query.exec()) return result;
    while(query.next()){
        result.push_back(query.value(0).toInt());
    }
    return result;
}

QVector<user_id> Database::q2_chat_members(const chat_id chatid, int start, int end)
{
    QVector<user_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 "
                                "LIMIT %4, %5;")
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_CHATID)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);

    if(!query.exec()) return result;
    while(query.next()){
        result.push_back(query.value(0).toInt());
    }
    return result;
}

QVector<chat_id> Database::q2_user_chats(const user_id userid, int start, int end)
{
    QVector<chat_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 "
                                "LIMIT %4, %5;")
            .arg(DB_USER_CHAT_CHATID)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_USERID)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value_1",   userid);

    if(!query.exec()) return result;
    while(query.next()){
        result.push_back(query.value(0).toInt());
    }
    return result;
}

QVector<msg_id> Database::q2_chat_msgs(const chat_id chatid, int start, int end)
{
    QVector<msg_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value_1 "
                                "LIMIT %4, %5;")
            .arg(DB_MESSAGE_MSGID)
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_MESSAGE_CHATID)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);

    if(!query.exec()) return result;
    while(query.next()){
        result.push_back(query.value(0).toInt());
    }
    return result;
}

QVector<user_id> Database::q2_users(const QString &text, int start, int end)
{
    QVector<user_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 LIKE :value "
                                "LIMIT %4, %5")
            .arg(DB_USER_USERID)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_NAME)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value",  text + '%');

    if(!query.exec()) return result;

    while (query.next()) {
        result.push_back(query.value(0).toInt());
    }

    return result;
}

QVector<chat_id> Database::q2_chats(const QString &text, int start, int end)
{
    QVector<chat_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 LIKE :value "
                                "LIMIT %4, %5")
            .arg(DB_CHAT_CHATID)
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_NAME)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value",  text + '%');

    if(!query.exec()) return result;

    while (query.next()) {
        result.push_back(query.value(0).toInt());
    }

    return result;
}

QVector<msg_id> Database::q2_msgs(const QString &text, int start, int end)
{
    QVector<msg_id> result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 LIKE :value "
                                "LIMIT %4, %5;")
            .arg(DB_MESSAGE_MSGID)
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_MESSAGE_TEXT)
            .arg(start)
            .arg(end);

    query.prepare(queryText);
    query.bindValue(":value",  text + '%');

    if(!query.exec()) return result;

    while (query.next()) {
        result.push_back(query.value(0).toInt());
    }

    return result;
}

QJsonObject Database::q2_json_chat(const chat_id chatid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2 "
                                "FROM %3 "
                                "WHERE %4 = :value_1;")
            .arg(DB_CHAT_NAME)
            .arg(DB_CHAT_COUNTER)
            .arg(DB_CHAT_TABLE)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("chatid", chatid);
    json.insert("name",   query.value(0).toString());
    json.insert("msg",    q3_msg_json(chatid, query.value(1).toInt() - 1));

    return json;
}

QJsonObject Database::q2_json_chat(const user_id userid, const chat_id chatid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2, %3 "
                                "FROM %4, %5 "
                                "WHERE %6 = :value_1 AND %7 = :value_2 AND %8 = :value_3;")
            .arg(DB_CHAT_NAME)
            .arg(DB_USER_CHAT_READ)
            .arg(DB_CHAT_COUNTER)
            .arg(DB_CHAT_TABLE)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID)
            .arg(DB_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1",   userid);
    query.bindValue(":value_2",   chatid);
    query.bindValue(":value_3",   chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("chatid", chatid);
    json.insert("name",   query.value(0).toString());
    json.insert("msg",    q3_msg_json(chatid, query.value(2).toInt() - 1));
    json.insert("read",   query.value(1).toInt());
    json.insert("count",  query.value(2).toInt() - query.value(1).toInt());

    return json;
}

QJsonObject Database::q2_json_user(const user_id userid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2, %3, %4 "
                                "FROM %5 "
                                "WHERE %6 = :value_1;")
            .arg(DB_USER_NAME)
            .arg(DB_USER_DATE)
            .arg(DB_USER_TIME)
            .arg(DB_USER_ONLINE)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1",   userid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("userid", userid);
    json.insert("name",   query.value(0).toString());
    json.insert("state",  query.value(1).toString() + " " + query.value(2).toString());
    json.insert("online",  query.value(3).toInt());

    return json;
}

QJsonObject Database::q2_json_user(const user_id userid, const chat_id chatid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2, %3, %4, %5 "
                                "FROM %6, %7 "
                                "WHERE %8 = :value_1 AND %9 = :value_2 AND %10 = :value_3;")
            .arg(DB_USER_NAME)
            .arg(DB_USER_DATE)
            .arg(DB_USER_TIME)
            .arg(DB_USER_CHAT_POSITION)
            .arg(DB_USER_ONLINE)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_CHAT_TABLE)
            .arg(DB_USER_USERID)
            .arg(DB_USER_CHAT_USERID)
            .arg(DB_USER_CHAT_CHATID);

    query.prepare(queryText);
    query.bindValue(":value_1",   userid);
    query.bindValue(":value_2",   userid);
    query.bindValue(":value_3",   chatid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("userid", userid);
    json.insert("name",   query.value(0).toString());
    json.insert("status", query.value(3).toInt());
    json.insert("state",  query.value(1).toString() + " " + query.value(2).toString());
    json.insert("online",  query.value(4).toInt());

    return json;
}

QString Database::getName(const user_id userid)
{
    QString result;

    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1 "
                                "FROM %2 "
                                "WHERE %3 = :value;")
            .arg(DB_USER_NAME)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value",     userid);

    if(!query.exec()) return result;
    if(!query.next()) return result;

    result = query.value(0).toString();

    return result;
}

QVector<user_id> Database::getAll(user_id userid)
{
    // Множественное включение
    QVector<user_id> result;
    QVector<chat_id> chats = q2_user_chats(userid, 0, 100000);
    for(auto e: chats){
        result << q2_chat_members(e);
    }
    return result;
}



QJsonObject Database::q3_msg_json(const chat_id chatid, const msg_id msgid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("SELECT %1, %2, %3, %4, %5, %6, %7, %8, %9 "
                                "FROM %10, %11 "
                                "WHERE %12 = %13 AND %14 = :value_1 AND %15 = :value_2;")
            .arg(DB_USER_USERID)
            .arg(DB_USER_NAME)
            .arg(DB_MESSAGE_DATE)
            .arg(DB_MESSAGE_TIME)
            .arg(DB_MESSAGE_TEXT)
            .arg(DB_MESSAGE_TYPE)
            .arg(DB_MESSAGE_STATUS)
            .arg(DB_MESSAGE_ANSWER)
            .arg(DB_MESSAGE_EDIT)
            .arg(DB_MESSAGE_TABLE)
            .arg(DB_USER_TABLE)
            .arg(DB_USER_USERID)
            .arg(DB_MESSAGE_USERID)
            .arg(DB_MESSAGE_CHATID)
            .arg(DB_MESSAGE_MSGID);

    query.prepare(queryText);
    query.bindValue(":value_1",   chatid);
    query.bindValue(":value_2",   msgid);

    _err_json(!query.exec(), query.lastError().text());
    _err_json(query.next() == 0, NOTHING_FOUND);

    QJsonObject json;
    json.insert("msgid",    msgid);
    json.insert("userid",   query.value(0).toInt());
    json.insert("name",     query.value(1).toString());
    json.insert("date",     query.value(2).toString());
    json.insert("time",     query.value(3).toString());
    json.insert("text",     query.value(4).toString());
    json.insert("type",     query.value(5).toInt());
    json.insert("status",   query.value(6).toInt());
    json.insert("answerid", query.value(7).toInt());
    json.insert("edit",     query.value(8).toInt());

    return json;
}

void Database::setOnline(const user_id userid)
{
    QSqlQuery query(m_database);
    QString queryText = QString("UPDATE %1 "
                                "SET %2 = :value_1 "
                                "WHERE %3 = :value_2;")
                        .arg(DB_USER_TABLE)
                        .arg(DB_USER_ONLINE)
                        .arg(DB_USER_USERID);

    query.prepare(queryText);
    query.bindValue(":value_1",   ONLINE);
    query.bindValue(":value_2",   userid);

    qDebug() << query.exec();
}


