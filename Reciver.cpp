#include "Reciver.h"

Reciver::Reciver(QString pUrl){
    connect(&ws,SIGNAL(connected()),this,SLOT(onConn()),Qt::QueuedConnection);
    connect(&ws,SIGNAL(textMessageReceived(QString)),this,SLOT(onText(QString)),Qt::QueuedConnection);
    connect(&socketBroad,SIGNAL(connected()),this,SLOT(onUdpConn()),Qt::QueuedConnection);
    connect(&socketBroad,SIGNAL(readyRead()),this,SLOT(onUpdBroad()),Qt::QueuedConnection);
    ws.open(QUrl(QString("ws://")+pUrl));
}

void Reciver::onConn(){
    BUG << ws.peerName() << ws.peerAddress() << ws.peerPort();
    BUG << ws.localAddress() << ws.localPort();
    //socketBroad.connectToHost("118.178.127.35",ws.localPort());
    //socketBroad.connectToHost(QHostAddress("58.33.80.115"),ws.localPort());
    socketBroad.bind(QHostAddress("58.33.80.115"), ws.localPort());
}

void Reciver::onUdpConn(){
    BUG << socketBroad.peerAddress() << socketBroad.peerPort();
    QHostAddress recvAddress = QHostAddress("192.168.1.6");
    quint16 recvPort = ws.localPort();
    //socketBroad.joinMulticastGroup(QHostAddress("58.33.80.115"));

    while(1){
        QByteArray data;
        BUG << socketBroad.readDatagram(data.data(),1000,&recvAddress,&recvPort);
        BUG << data << data.count();
        BUG << socketBroad.receiveDatagram(1000).data();
        QThread::sleep(1);
    }

}

void Reciver::onText(QString pMsg){
    QJsonDocument jsonDoc = QJsonDocument::fromJson(pMsg.toLatin1());
    QJsonArray    jsonArr = jsonDoc.array();
    BUG << jsonArr;
    emit toWindow(jsonArr);
}

void Reciver::onUpdBroad(){
    BUG << socketBroad.readAll();
}
