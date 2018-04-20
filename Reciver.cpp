#include "Reciver.h"

Reciver::Reciver(QString pUrl){
    connect(&ws,SIGNAL(connected()),this,SLOT(onConn()),Qt::QueuedConnection);
    connect(&ws,SIGNAL(textMessageReceived(QString)),this,SLOT(onText(QString)),Qt::QueuedConnection);
    ws.open(QUrl(QString("ws://")+pUrl));
}

void Reciver::onConn(){
    BUG;
}

void Reciver::onText(QString pMsg){
    QJsonDocument jsonDoc = QJsonDocument::fromJson(pMsg.toLatin1());
    QJsonObject   jsonObj = jsonDoc.object();
    BUG << jsonObj;
    emit toWindow(jsonObj);
}


