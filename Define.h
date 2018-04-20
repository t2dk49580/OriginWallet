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

#define BUG qDebug()<<__FUNCTION__<<__LINE__
#define SETXF(A,B) QString("%1").arg(A, 0, 'f', B)

#define GETMD5(A) QCryptographicHash::hash(A,QCryptographicHash::Md5).toHex()
#define GETADDR(A) QByteArray("0x")+QCryptographicHash::hash(A,QCryptographicHash::Keccak_256).toHex().left(40)
#define GETSHA256(A) QCryptographicHash::hash(A,QCryptographicHash::Sha256).toHex()

#endif // DEFINE_H
