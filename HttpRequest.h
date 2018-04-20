#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QCoreApplication>
#include <QSettings>
#include <curl/curl.h>
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
#include "Define.h"
#include "Password.h"

using namespace std;

class HttpRequest {
public:
    HttpRequest(){}
    ~HttpRequest(){}

    static size_t call_wirte_func(const char *ptr, size_t size, size_t nmemb, std::string *stream)
    {
        size_t len = size * nmemb;
        stream->append(ptr, len);
        return len;
    }
    static size_t header_callback(const char  *ptr, size_t size, size_t nmemb, std::string *stream)
    {
        size_t len = size * nmemb;
        stream->append(ptr, len);
        return len;
    }

    static bool getbuffer(string pUrl, string &szbuffer)
    {
        cout << pUrl.c_str() << endl;
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_wirte_func);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &szbuffer);
            curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10 );
            curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 10 );
            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK){
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
                return false;
            }
            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        return true;
    }

    static bool postbuffer(string postFields, string url,string &szbuffer) {
        cout << url.c_str() << " " <<  postFields.c_str() << endl;
        CURL *curl_handle = NULL;
        CURLcode res = CURLE_FAILED_INIT;
        string szheader_buffer;
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Accept-Encoding: gzip, deflate");
        chunk = curl_slist_append(chunk, "Accept-Language: en-US,en;q=0.8");
        chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.130 Safari/537.36");

        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();

        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "cookie.dat");
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, "cookie.dat");
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, call_wirte_func);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &szbuffer);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_callback);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &szheader_buffer);
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postFields.c_str());

        curl_easy_setopt( curl_handle, CURLOPT_TIMEOUT, 10 );
        curl_easy_setopt( curl_handle, CURLOPT_CONNECTTIMEOUT, 10 );

        res = curl_easy_perform(curl_handle);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(chunk);
        chunk = NULL;
        curl_global_cleanup();
        if (res == CURLE_OK)
        {
            return true;
        }
        return false;
    }

    static QByteArray post(QString url,QString field){
        QByteArray ret;
        string buffer;
        if(postbuffer(field.toStdString(),url.toStdString(),buffer)){
            ret = buffer.c_str();
        }
        return ret;
    }
    static QByteArray get(QString url){
        QByteArray ret;
        string buffer;
        if(getbuffer(url.toStdString(),buffer)){
            ret = buffer.c_str();
        }
        return ret;
    }

    static QByteArray qtGet(const QString &strUrl)
    {
        BUG;

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

    static QString getTaget(QString type,QString pubkey,QString name,QString code,QString arg,QString sig) {
        QString data = type + "$" + pubkey + "$" + name + "$" + code + "$" + arg;
        QString block = sig + "&" + data;
        return block;
    }

    static QString docmd(QString type,QString pubkey,QString prikey,QString name,QString func,QString arg) {
        QString msg = type + "$" + pubkey + "$" + name + "$" + func + "$" + arg;
        QByteArray hash = GETSHA256(msg.toLatin1());
        sign(prikey.toLatin1().data(),hash.data());
        QByteArray sig = getSign();
        QString block = getTaget(type, pubkey, name, func, arg, sig);
        return block;
    }

    static QJsonObject doMethodGet(Password &psd,QString url,QString pContract,QString pMethod,QStringList pArg){
        BUG;
        QString arg;
        for(auto cur:pArg)
            arg.append(cur).append("?");
        arg.remove(arg.count()-1,1);
        QString block = pContract+"$"+pMethod+"$"+arg+"$"+psd.pubkey;
        QByteArray result = get(url+"/"+block);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(result);
        QJsonObject   jsonObj = jsonDoc.object();
        BUG << jsonObj;
        return jsonObj;
    }

    static QByteArray doMethodSet(Password &psd,QString url,QString pContract,QString pMethod,QStringList pArg){
        BUG;
        QString arg;
        for(auto cur:pArg)
            arg.append(cur).append("?");
        arg.remove(arg.count()-1,1);
        QString block = docmd("method",psd.pubkey,psd.prikey,pContract,pMethod,arg);
        return get(url+"/"+block);
    }

    static QByteArray doDeploy(Password &psd,QString url,QString pContract,QString pCode,QStringList pArg){
        BUG;
        QString arg;
        for(auto cur:pArg)
            arg.append(cur).append("?");
        arg.remove(arg.count()-1,1);
        QString block = docmd("deploy",psd.pubkey,psd.prikey,pContract,pCode,arg);
        return post(url,block);
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
