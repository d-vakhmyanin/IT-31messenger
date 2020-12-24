#include "config.h"

#include <QTextStream>

Config::Config() : QObject()
{

}

void Config::Read(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        QString line;
        QStringList list;
        while(!(line = stream.readLine()).isEmpty()){
            list = line.split('=');
            if(list.size() != 2) continue;
            list[0] = list[0].trimmed();
            list[1] = list[1].trimmed();
            if(list[0] == "database") this->database = list[1];
            else if(list[0] == "port") this->port = list[1].toInt();
        }
        file.close();
    }
}
