#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVector>

using user_id = int;
using chat_id = int;
using msg_id = int;

#define DB_SCHEMA               "dbmsg"

#define DB_USER_TABLE           "muser"
#define DB_USER_USERID          "u_userid"
#define DB_USER_LOGIN           "u_login"
#define DB_USER_PASSWORD        "u_password"
#define DB_USER_NAME            "u_name"
#define DB_USER_INFO            "u_info"
#define DB_USER_DATE            "u_date"
#define DB_USER_TIME            "u_time"
#define DB_USER_ONLINE          "u_online"

#define DB_CHAT_TABLE           "mchat"
#define DB_CHAT_CHATID          "c_chatid"
#define DB_CHAT_NAME            "c_name"
#define DB_CHAT_INFO            "c_info"
#define DB_CHAT_TYPE            "c_type"
#define DB_CHAT_COUNTER         "c_counter"

#define DB_USER_CHAT_TABLE      "muserchat"
#define DB_USER_CHAT_CHATID     "uc_chatid"
#define DB_USER_CHAT_USERID     "uc_userid"
#define DB_USER_CHAT_POSITION   "uc_position"
#define DB_USER_CHAT_READ       "uc_read"

#define DB_MESSAGE_TABLE        "mmsg"
#define DB_MESSAGE_MSGID        "m_msgid"
#define DB_MESSAGE_CHATID       "m_chatid"
#define DB_MESSAGE_USERID       "m_userid"
#define DB_MESSAGE_TEXT         "m_text"
#define DB_MESSAGE_DATE         "m_date"
#define DB_MESSAGE_TIME         "m_time"
#define DB_MESSAGE_ANSWER       "m_anwid"
#define DB_MESSAGE_STATUS       "m_status"
#define DB_MESSAGE_TYPE         "m_type"
#define DB_MESSAGE_EDIT         "edit"

#define DB_CONTACT_TABLE        "mcontact"
#define DB_CONTACT_USERID_1     "r_userid_1"
#define DB_CONTACT_USERID_2     "r_userid_2"
#define DB_CONTACT_STATUS       "r_status"

#define OFFLINE 0
#define ONLINE  1

#define SQL_ERROR               "SQL error."
#define AFFECTED_ROWS_ERROR     "Affected rows zero."
#define NOTHING_FOUND           "Nothing found."
#define POSITION_ERROR          "Not enough authority."
#define _err_json(e, t)         if(e) { QJsonObject j; j.insert("error", t); return j; }
#define _return_true_json()     { QJsonObject j; j.insert("error", "none"); return j; }
#define _return_json(j)         { j.insert("error", "none"); return j; }

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    explicit Database(QString path, QObject *parent = nullptr);
    //void create(QString host, int port, QString userName, QString password);

/**/QJsonObject q2_create_user          (const QString &login, const QString &password, const QString &name, const QString &info);
/**/QJsonObject q2_remove_user          (const user_id userid, const QString &password);
/**/QJsonObject q2_update_user          (const user_id userid, const QString &param, const QString &value);
    QJsonObject q2_supdate_user         (const user_id userid, const QString &param, const QString &value, const QString &password);
/**/QJsonObject q2_get_user             (const user_id userid, const user_id _userid);
/**/QJsonObject q2_is_exist_user        (const QString &login);
/**/QJsonObject q2_connect_user         (const QString &login, const QString &password);
/**/QJsonObject q2_disconnect_user      (const user_id userid, const QString &date, const QString &time);
/**/QJsonObject q2_get_chats_user       (const user_id userid, int start, int end);
/**/QJsonObject q2_reconnect_user       (const user_id userid);

/**/QJsonObject q2_create_chat          (const user_id userid, const QString &name, const QString &info, int type);
    QJsonObject q2_remove_chat          (const chat_id chatid);
    QJsonObject q2_update_chat          (const chat_id chatid, const QString &param, const QString &value);
    QJsonObject q2_update_user_chat     (const chat_id chatid, const int value, const user_id userid);
/**/QJsonObject q2_add_user_chat        (const chat_id chatid, const user_id userid, int position);
    QJsonObject q2_remove_user_chat     (const chat_id chatid, const user_id userid);
    QJsonObject q2_get_chat             (const user_id userid, const chat_id chatid);
/**/QJsonObject q2_get_users_chat       (const chat_id chatid, int start, int end);
/**/QJsonObject q2_get_msgs_chat        (const chat_id chatid, int start, int end);

/**/QJsonObject q2_create_msg           (const user_id userid, const chat_id chatid, const QString &text, const QString &date, const QString &time, const msg_id answer, const int type);
    QJsonObject q2_remove_msg           (const chat_id chatid, const msg_id msgid);
    QJsonObject q2_update_msg           (const chat_id chatid, const msg_id msgid, const QString &text, const QString &date, const QString &time);
    QJsonObject q2_read_msg             (const user_id userid, const chat_id chatid, const msg_id msgid);

    QJsonObject q2_add_contact          (const user_id userid, const user_id contactid);
    QJsonObject q2_remove_contact       (const user_id userid, const user_id contactid);
    QJsonObject q2_block_contact        (const user_id userid, const user_id contactid, const int state);
    QJsonObject q2_get_contact          (const user_id userid, const user_id contactid);
    QJsonObject q2_get_contacts         (const user_id userid, const user_id contactid);

/**/QJsonObject q2_find_user            (const user_id userid, const QString& text, int start, int end);
/**/QJsonObject q2_find_chat            (const QString& text, int start, int end);
/**/QJsonObject q2_find_msg             (const QString& text, const chat_id chatid, int start, int end);

    bool open() {return m_database.open();}
    void close() {m_database.close();}

    bool error() const;

    int q2_autoincrement(const QString table, QString* error);
    int q2_user_position(const user_id userid, const chat_id chatid, QString* error);
    int q2_get_chat_counter(const chat_id chatid, QString* error);
    int q2_get_members(const chat_id chatid, QString* error);
    QJsonObject q2_get_last_message(const chat_id chatid, QString* error);

    QVector<user_id> q2_chat_members  (const chat_id chatid);
    QVector<user_id> q2_chat_members  (const chat_id chatid, int start, int end);
    QVector<chat_id> q2_user_chats    (const user_id userid, int start, int end);
    QVector<msg_id>  q2_chat_msgs     (const chat_id chatid, int start, int end);

    QVector<user_id> q2_users         (const QString& text, int start, int end);
    QVector<chat_id> q2_chats         (const QString& text, int start, int end);
    QVector<msg_id>  q2_msgs          (const QString& text, int start, int end);

    QJsonObject q2_json_chat(const chat_id chatid);
    QJsonObject q2_json_chat(const user_id userid, const chat_id chatid);

    QJsonObject q2_json_user(const user_id userid);
    QJsonObject q2_json_user(const user_id userid, const chat_id chatid);

    QString getName(const user_id userid);

    QVector<user_id> getAll(user_id userid);

    QJsonObject q3_msg_json(const chat_id, const msg_id);

    void setOnline(const user_id userid);
private:
    QSqlDatabase m_database;

    bool m_error = false;
    QString m_errorText;
};

#endif // DATABASE_H
