#include <QCoreApplication>

#include "config.h"
#include "webserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Config cfg;
    cfg.Read("config.cfg");
    qDebug() << cfg.port;
    qDebug() << cfg.database;
    WebServer server("MSGServer", QWebSocketServer::SslMode::NonSecureMode);
    Database db(cfg.database);
    //db.create("127.0.0.1", 3306, "root", "policeman");
    server.setDatabase(&db);
    server.start(cfg.port);

    a.exec();

    server.stop();
    return 0;
}
