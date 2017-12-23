#include "instagramrequestv2.h"
#include "instagramv2_p.h"
#include "../cripto/hmacsha.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>

InstagramRequestv2::InstagramRequestv2(QNetworkReply *reply,
                                       QObject *parent):
    QObject(parent),
    m_reply(reply)
{
    QObject::connect(reply, &QNetworkReply::finished, this, &InstagramRequestv2::finishGetUrl);
    QObject::connect(reply, &QNetworkReply::uploadProgress, this, &InstagramRequestv2::uploadProgress);
}



InstagramRequestv2 *Instagramv2Private::fileRequest(QString endpoint, QString boundary, QByteArray data)
{
    QUrl url(Constants::apiUrl()+endpoint);
    QNetworkRequest request(url);

    request.setRawHeader("Connection","close");
    request.setRawHeader("Accept","*/*");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data; boundary="+boundary.toUtf8());
    request.setHeader(QNetworkRequest::ContentLengthHeader,data.size());
    request.setHeader(QNetworkRequest::UserAgentHeader,Constants::userAgent());

    request.setRawHeader("Cookie2","$Version=1");
    request.setRawHeader("Accept-Language","en-US");
    request.setRawHeader("Accept-Encoding","gzip");

    QNetworkReply *mReply = this->m_manager->post(request,data);

    return new InstagramRequestv2(mReply, this);
}

InstagramRequestv2 *Instagramv2Private::request(QString endpoint, QByteArray post)
{
    QUrl url(Constants::apiUrl()+endpoint);
    QNetworkRequest request(url);

    request.setRawHeader("Connection","close");
    request.setRawHeader("Accept","*/*");
    request.setRawHeader("Content-type","application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie2","$Version=1");
    request.setRawHeader("Accept-Language","en-US");
    request.setRawHeader("User-Agent",Constants::userAgent());

    QNetworkReply *mReply = this->m_manager->post(request,post);

    return new InstagramRequestv2(mReply, this);
}

void InstagramRequestv2::finishGetUrl()
{
    //this->m_reply->deleteLater();
    QNetworkReply *nReply = qobject_cast<QNetworkReply *>(sender());
    QVariant answer = QString::fromUtf8(nReply->readAll());
    if(answer.toString().length() > 1)
    {
        Q_EMIT replyStringReady(answer);
    }
    nReply->deleteLater();
}

void Instagramv2Private::saveCookie() const
{
    QList<QNetworkCookie> list =
        m_manager->cookieJar()->cookiesForUrl(QUrl(Constants::apiUrl()+"/"));

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
    QByteArray hash = hmac->hash(data_string.toUtf8(), Constants::isSigKey());

    return QString("ig_sig_key_version="+Constants::sigKeyVersion()+"&signed_body="+hash.toHex()+"."+data_string.toUtf8());
}

