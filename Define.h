#ifndef DEFINE_H
#define DEFINE_H

#include <QFile>
#include <QDebug>
#include <QMutex>
#include <QSettings>
#include <QtConcurrent>
#include <QThreadPool>
#include <QTcpServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QImage>

#define BUG qDebug()<<__FUNCTION__<<__LINE__
#define SETXF(A,B) QString("%1").arg(A, 0, 'f', B)

#define GETMD5(A) QCryptographicHash::hash(A,QCryptographicHash::Md5).toHex()
#define GETADDR(A) QCryptographicHash::hash(QByteArray::fromHex(A),QCryptographicHash::Keccak_256).toHex().right(40)
#define GETSHA256(A) QCryptographicHash::hash(A,QCryptographicHash::Sha256).toHex()

struct onnBlock {
    QByteArray blockIndex;
    QByteArray blockTimestamp;
    QByteArray blockHashPrev;
    QByteArray blockHash;
    QByteArray blockData;
    QByteArray blockMaker;
    QByteArray blockMakerSign;
};

#endif // DEFINE_H
