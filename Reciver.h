#ifndef RECIVER_H
#define RECIVER_H

#include <QObject>
#include "Define.h"

class Reciver : public QObject
{
    Q_OBJECT
public:
    explicit Reciver(QString pUrl);

    QWebSocket ws;
signals:
    void toWindow(QJsonObject);
public slots:
    void onConn();
    void onText(QString);
};

#endif // RECIVER_H
