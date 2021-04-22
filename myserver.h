//
// Created by 48562 on 14.03.2021.
//

#ifndef UNTITLED11_MYSERVER_CPP
#define UNTITLED11_MYSERVER_CPP

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QFile>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>
#include <QString>



class myServer : public QTcpServer{
Q_OBJECT
public:
    myServer();

    ~myServer();

    QTcpSocket *socket;
    QByteArray Data;

    QJsonDocument doc;
    QJsonParseError docError;

    QSqlDatabase db;

public slots:

    void startServer();

    // готовность сокета к чтению
    void sockReady();
    // при отключении
    void sockDisk();
    QString SQLQuery(const QString &sqlquery);
// действия при соединении
protected:
    void incomingConnection(qintptr handle) override;



};

#endif //UNTITLED11_MYSERVER_CPP

