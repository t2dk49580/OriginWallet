#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QCoreApplication>
#include <QSettings>
#include <iostream>
#include <QList>
#include <QString>
#include <stdio.h>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtConcurrent>
#include <QThreadPool>
#include <QTime>
#include <QCryptographicHash>
#include <QFile>
#include <QNetworkRequest>
#include "Define.h"
#include "Password.h"

using namespace std;

class NetRequest : public QThread {
    Q_OBJECT
public:
    NetRequest(){
        CONN(this,SIGNAL(started()),this,SLOT(onStart()));
    }
    ~NetRequest(){}

signals:
    void doRsponse(QString);
public slots:
    void onStart(){
        mNetGet = new QNetworkAccessManager();
        mNetPost = new QNetworkAccessManager();
        connect(mNetGet, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onFinish(QNetworkReply*)),Qt::QueuedConnection);
        connect(mNetPost, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onFinish(QNetworkReply*)),Qt::QueuedConnection);
    }
    void onGet(QString pUrl,QString pData){
        cout << (pUrl+"/"+pData).toLatin1().data() << endl;
        mNetGet->get(QNetworkRequest(pUrl+"/"+pData));
    }
    void onPost(QString pUrl,QString pData){
        QNetworkRequest qnr(pUrl);
        qnr.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        mNetPost->post(qnr,pData.toLatin1());
    }
    void onFinish(QNetworkReply *pReply){
        QByteArray data = pReply->readAll();
        BUG << QTime::currentTime().toString("ss zzz") << data << endl;
        emit doRsponse(data);
        //pReply->deleteLater();
        delete pReply;
        pReply = Q_NULLPTR;
    }

private:
    QNetworkAccessManager *mNetGet;
    QNetworkAccessManager *mNetPost;
};

class HttpRequest {
public:
    HttpRequest(){}
    ~HttpRequest(){}

    static QByteArray qtGet(const QString &strUrl)
    {
        BUG << strUrl;

        const QUrl url = QUrl::fromUserInput(strUrl);
        QNetworkAccessManager m_qnam;
        QByteArray replyData;

        QNetworkRequest qnr(url);
        QNetworkReply* reply = m_qnam.get(qnr); //m_qnam是QNetworkAccessManager对象

        QEventLoop eventLoop;
        QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        //eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
        eventLoop.exec();

        replyData = reply->readAll();

        reply->deleteLater();
        reply = nullptr;

        return replyData;
    }

    static QByteArray qtPost(const QString &strUrl,QByteArray pData)
    {
        BUG;

        const QUrl url = QUrl::fromUserInput(strUrl);
        QNetworkAccessManager m_qnam;
        QByteArray replyData;

        QNetworkRequest qnr(url);
        qnr.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        QNetworkReply* reply = m_qnam.post(qnr,pData); //m_qnam是QNetworkAccessManager对象

        QEventLoop eventLoop;
        QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        //eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
        eventLoop.exec();

        replyData = reply->readAll();

        reply->deleteLater();
        reply = nullptr;

        return replyData;
    }

    static QString getTaget(QString type,QString pubkey,QString name,QString code,QString arg,QString pTime,QString sig) {
        QString data = type + "$" + pubkey + "$" + name + "$" + code + "$" + arg + "$" + pTime;
        QString block = sig + "&" + data;
        return block;
    }

    static QString docmd(QString type,QString pubkey,QString prikey,QString name,QString func,QString arg) {
        QString curTimeStamp = QString::number(QDateTime::currentMSecsSinceEpoch());
        QString msg = type + "$" + pubkey + "$" + name + "$" + func + "$" + arg + "$" + curTimeStamp;
        QByteArray hash = GETSHA256(msg.toLatin1());
        sign(prikey.toLatin1().data(),hash.data());
        QByteArray sig = getSign();
        sig = sig.left(128);
        QString block = getTaget(type, pubkey, name, func, arg, curTimeStamp, sig);
        return block;
    }

    static QJsonArray doMethodGet(Password &psd,QString url,QString pContract,QString pMethod,QString pArg){
        QString block = pContract+"$"+pMethod+"$"+pArg+"$"+psd.pubkey;
        BUG << block;
        QByteArray result = qtGet(url+"/"+block);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(result);
        QJsonArray    jsonArr = jsonDoc.array();
        BUG << jsonArr;
        return jsonArr;
    }

    static QJsonDocument doMethodGet1(Password &psd,QString url,QString pContract,QString pMethod,QString pArg){
        QString block = pContract+"$"+pMethod+"$"+pArg+"$"+GETADDR(psd.pubkey);
        BUG << block;
        QByteArray result = qtGet(url+"/"+block);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(result);
        BUG << jsonDoc;
        return jsonDoc;
    }

    static QByteArray doMethodSet(Password &psd,QString url,QString pContract,QString pMethod,QString pArg){
        QString block = docmd("method",psd.pubkey,psd.prikey,pContract,pMethod,pArg);
        BUG << url << block;
        //return qtGet(url+"/"+block);
        return qtPost(url,block.toLatin1());
    }

    static QByteArray doDeploy(Password &psd,QString url,QString pContract,QString pCode,QString pArg){
        QString block = docmd("deploy",psd.pubkey,psd.prikey,pContract,pCode,pArg);
        BUG << url << block;
        return qtPost(url,block.toLatin1());
    }

    QByteArray getSignHash(Password &psd,QByteArray pHash){
        sign(psd.prikey.data(),pHash.data());
        return QByteArray(getSign()).left(128);
    }

    QByteArray calcHash(onnBlock pBlock){
        return GETSHA256(pBlock.blockIndex+\
                                  pBlock.blockTimestamp+\
                                  pBlock.blockHashPrev+\
                                  pBlock.blockData+\
                                  pBlock.blockMaker).toHex();
    }

    static QByteArray toString(onnBlock pBlock){
        return pBlock.blockIndex+"@"+pBlock.blockTimestamp+"@"+pBlock.blockHashPrev+"@"+pBlock.blockHash\
                +"@"+pBlock.blockData+"@"+pBlock.blockMaker+"@"+pBlock.blockMakerSign;
    }

    static QByteArray doSignMethod(Password &psd,QString pContract,QString pMethod,QString pArg){
        QString block = docmd("method",psd.pubkey,psd.prikey,pContract,pMethod,pArg);
        return block.toLatin1();
    }

    static QByteArray doSignDeploy(Password &psd,QString pContract,QString pMethod,QString pArg){
        QString block = docmd("deploy",psd.pubkey,psd.prikey,pContract,pMethod,pArg);
        return block.toLatin1();
    }

    static QByteArray doMake(Password &psd,QString pIndex,QString pHashPrev,QString pData){
        onnBlock curBlock;
        curBlock.blockIndex = pIndex.toLatin1();
        curBlock.blockTimestamp = QByteArray::number(QDateTime::currentMSecsSinceEpoch());//QDateTime::currentMSecsSinceEpoch()
        curBlock.blockHashPrev = pHashPrev.toLatin1();
        curBlock.blockData = pData.toLatin1();
        curBlock.blockMaker = psd.pubkey;
        curBlock.blockHash = GETSHA256(curBlock.blockIndex+\
                                       curBlock.blockTimestamp+\
                                       curBlock.blockHashPrev+\
                                       curBlock.blockData+\
                                       curBlock.blockMaker).toHex();
        sign(psd.prikey.data(),curBlock.blockHash.data());
        curBlock.blockMakerSign = QByteArray(getSign()).left(128);
        return toString(curBlock);
    }
};

class BlockBroadcast : public QObject {
    Q_OBJECT
public:
    BlockBroadcast();
    ~BlockBroadcast();

public slots:
    void start();
    void doReal();
    void doSync();
};



#endif // HTTPREQUEST_H
