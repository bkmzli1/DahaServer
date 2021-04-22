#include "myserver.h"


myServer::myServer() {}

myServer::~myServer() {}

void myServer::startServer() {

    if (this->listen(QHostAddress::Any, 80)) {

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("nifi.db");


        if (db.open()) { qDebug() << "bd is open"; } else { qDebug() << "bd not open"; }
        QSqlQuery *query = new QSqlQuery(db);
        QString createTableArray[50] = {
                "create table users ( post_id TEXT not null primary key, name TEXT not null, role TEXT not null, password TEXT not null, userOnOf BLOB not null, userpassvalidity BLOB not null, userblocked BLOB not null, email TEXT ,user_name TEXT, surname TEXT, patronymic TEXT);",
                "CREATE TABLE teach_works( post_id TEXT NOT NULL PRIMARY KEY, user_id TEXT NOT NULL, subject TEXT NOT NULL, condition BLOB NOT NULL DEFAULT 0, mark INTEGER, comment TEXT NOT NULL, code TEXT );",
                "INSERT INTO users (post_id, name, role, password, userOnOf, userpassvalidity, userblocked, email, user_name, surname, patronymic) VALUES ('id', 'root', 'ADMIN', 'root', '0', '0', '0', 'email', 'root1', 'root2', 'root3');",
                "INSERT INTO users (post_id, name, role, password, userOnOf, userpassvalidity, userblocked, email, user_name, surname, patronymic) VALUES ('id4', 'prepod2', 'PREPOD', 'prepod', '0', '0', '0', 'email', 'prepod21', 'prepod22', 'prepod23');",
                "INSERT INTO users (post_id, name, role, password, userOnOf, userpassvalidity, userblocked, email, user_name, surname, patronymic) VALUES ('id3', 'prepod', 'PREPOD', 'prepod', '0', '0', '0', 'email', 'prepod1', 'prepod2', 'prepod3');",
                "INSERT INTO users (post_id, name, role, password, userOnOf, userpassvalidity, userblocked, email, user_name, surname, patronymic) VALUES ('id2', 'user', 'USER', 'user', '0', '0', '0', 'email', 'user1', 'user2', 'user3');"};

        for (int i = 0; i < sizeof(createTableArray) / sizeof(createTableArray[0]); ++i) {
            qDebug() << i;
            if (query->exec(createTableArray[i])) {
                qDebug() << "create";
            } else {
                qDebug() << "no create";
            }
        }


        qDebug() << "listen";
    } else {
        qDebug() << "not listen";
    }
}

void myServer::sockReady() {
    Data = socket->readAll();

    qDebug() << "data:" << Data;
    doc = QJsonDocument::fromJson(Data, &docError);
    qDebug() << "fromJson";
    if (docError.errorString().toInt() == QJsonParseError::NoError) {
        QString type = doc.object().value("type").toString();

        if (type == "auth") {
            QString userNameLogin = doc.object().value("userName").toString();
            QString passwordLogin = doc.object().value("password").toString();
            QSqlQuery *query = new QSqlQuery(db);
            QString out = SQLQuery(
                    "SELECT t.* FROM users t where name='" + userNameLogin + "' and  password='" + passwordLogin +
                    "';");
            QByteArray textTemp = out.toUtf8();

            socket->write(textTemp);
        }
        if (type == "prepod") {
            QString userNameLogin = doc.object().value("userName").toString();
            QString passwordLogin = doc.object().value("password").toString();
            QString role = doc.object().value("role").toString();
            QSqlQuery *query = new QSqlQuery(db);
            QString out = SQLQuery(
                    "SELECT t.* FROM users t where name='" + userNameLogin + "' and role='" + role +
                    "' and password='" + passwordLogin +
                    "';");

            QByteArray textTemp = out.toUtf8();
            if (textTemp != "[\n]\n") {
                QSqlQuery *query = new QSqlQuery(db);
                if (query->exec("SELECT t.* FROM users t where  role='PREPOD';")) {
                    QString out  = "[{";
                    int i = 0;

                    while (query->next()) {
                        out = out + " \"prepod" + QString::number(i) + "\":{ \"user_name\":\" " + query->value(8).toString() +
                              "\", \"surname\":\" " + query->value(9).toString() + "\", \"patronymic\":\" " +
                              query->value(10).toString() + "\",\"post_id\":\" " + query->value(5).toString() +
                              "\"},";
                        i += 1;
                    }
                    out.resize(out.size() - 1);

                    out = out + "}]";
                    qDebug() << out;
                    textTemp = out.toUtf8();
                    socket->write(textTemp);
                }

            } else {
                socket->write(textTemp);
            }

        }


        /*  socket->write(qByteArray);
          socket->waitForBytesWritten(500);*/
    } else {
        socket->write(R"({"error":"ERROR JSON"})");
        socket->waitForBytesWritten(500);
    }

}

void myServer::sockDisk() {
    qDebug() << "Disconnect";
    socket->deleteLater();
}

void myServer::incomingConnection(qintptr handle) {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(handle);

    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    if (disconnect()) {
        qDebug() << handle << "client disconnect()";
        connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisk()));
    }


    qDebug() << "Send client connect status - Yes";
}

QString myServer::SQLQuery(const QString &sqlquery) {
    QSqlQuery *query = new QSqlQuery(db);
    qDebug() << sqlquery;
    query->setForwardOnly(true);
    if (!query->exec(sqlquery))
        return QString();

    QJsonDocument json;
    QJsonArray recordsArray;

    while (query->next()) {
        QJsonObject recordObject;
        for (int x = 0; x < query->record().count(); x++) {


            recordObject.insert(query->record().fieldName(x),
                                QJsonValue::fromVariant(query->value(x)));

        }
        recordsArray.push_back(recordObject);
    }
    json.setArray(recordsArray);

    return json.toJson();
}
