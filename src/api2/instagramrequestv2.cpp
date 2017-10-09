#include "instagramrequestv2.h"
#include "../cripto/hmacsha.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>



InstagramRequestv2::InstagramRequestv2(QObject *parent) : QObject(parent)
{
    this->m_data_path =  QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    this->m_manager = new QNetworkAccessManager();
    this->m_jar = new QNetworkCookieJar;

    if(!m_data_path.exists())
    {
        m_data_path.mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }
}



void InstagramRequestv2::fileRquest(QString endpoint, QString boundary, QByteArray data)
{
    QFile f(m_data_path.absolutePath()+"/cookies.dat");
    f.open(QIODevice::ReadOnly);
    QDataStream s(&f);

    QUrl url(API_URL+endpoint);
    QNetworkRequest request(url);

    while(!s.atEnd()){
        QByteArray c;
        s >> c;
        QList<QNetworkCookie> list = QNetworkCookie::parseCookies(c);
        if(list.count() > 0)
        {
            this->m_jar->insertCookie(list.at(0));
        }
    }

    request.setRawHeader("Connection","close");
    request.setRawHeader("Accept","*/*");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data; boundary="+boundary.toUtf8());
    request.setHeader(QNetworkRequest::ContentLengthHeader,data.size());
    request.setHeader(QNetworkRequest::UserAgentHeader,USER_AGENT);

    request.setRawHeader("Cookie2","$Version=1");
    request.setRawHeader("Accept-Language","en-US");
    request.setRawHeader("Accept-Encoding","gzip");

    this->m_manager->setCookieJar(this->m_jar);
    QNetworkReply *mReply = this->m_manager->post(request,data);


    QObject::connect(mReply, &QNetworkReply::finished, this, &InstagramRequestv2::finishGetUrl);
    QObject::connect(this->m_manager, &QNetworkAccessManager::finished, this, &InstagramRequestv2::saveCookie);
}

void InstagramRequestv2::request(QString endpoint, QByteArray post)
{
    QFile f(m_data_path.absolutePath()+"/cookies.dat");
    f.open(QIODevice::ReadOnly);
    QDataStream s(&f);

    QUrl url(API_URL+endpoint);
    QNetworkRequest request(url);

    while(!s.atEnd()){
        QByteArray c;
        s >> c;
        QList<QNetworkCookie> list = QNetworkCookie::parseCookies(c);
        if(list.count() > 0)
        {
            this->m_jar->insertCookie(list.at(0));
        }
    }

    request.setRawHeader("Connection","close");
    request.setRawHeader("Accept","*/*");
    request.setRawHeader("Content-type","application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie2","$Version=1");
    request.setRawHeader("Accept-Language","en-US");
    request.setRawHeader("User-Agent",USER_AGENT.toUtf8());

    this->m_manager->setCookieJar(this->m_jar);
    this->m_reply = this->m_manager->post(request,post);
    connections<<m_reply;

    QObject::connect(this->m_reply, &QNetworkReply::finished, this, &InstagramRequestv2::finishGetUrl);
    QObject::connect(this->m_manager, &QNetworkAccessManager::finished, this, &InstagramRequestv2::saveCookie);
}

void InstagramRequestv2::finishGetUrl()
{
    //this->m_reply->deleteLater();
    QNetworkReply *nReply = qobject_cast<QNetworkReply *>(sender());
    QVariant answer = QString::fromUtf8(nReply->readAll());
    if(answer.toString().length() > 1)
    {
        emit replyStringReady(answer);
    }
    connections.removeAll(nReply);
    nReply->deleteLater();
}

void InstagramRequestv2::saveCookie()
{
    QList<QNetworkCookie> list =
        m_manager->cookieJar()->cookiesForUrl(QUrl(API_URL+"/"));

    QFile f(m_data_path.absolutePath()+"/cookies.dat");
    f.open(QIODevice::ReadWrite);
    for(int i = 0; i < list.size(); ++i){
        QDataStream s(&f);
        s << list.at(i).toRawForm();
    }

    f.close();
}


QString InstagramRequestv2::generateSignature(QJsonObject data)
{
    QJsonDocument data_doc(data);
    QString data_string(data_doc.toJson(QJsonDocument::Compact));

//Fix to image config string
    data_string.replace("\"crop_center\":[0,0]","\"crop_center\":[0.0,-0.0]");

    HmacSHA *hmac = new HmacSHA();
    QByteArray hash = hmac->hash(data_string.toUtf8(), IS_SIG_KEY.toUtf8());

    return QString("ig_sig_key_version="+SIG_KEY_VERSION+"&signed_body="+hash.toHex()+"."+data_string.toUtf8());
}

