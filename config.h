#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QFile>

class Config : QObject
{
    Q_OBJECT
public:
    Config();
    void Read(QString filename);

    QString database = "C:/Users/Admin/Documents/build-WebServer_0_2-Desktop_Qt_5_15_1_MSVC2019_64bit-Release/real/dbmsg.db";
    int port = 8000;
};

#endif // CONFIG_H
