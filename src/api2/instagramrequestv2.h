#ifndef INSTAGRAMREQUESTV2_H
#define INSTAGRAMREQUESTV2_H

#include <QDir>
#include <QNetworkAccessManager>
#include <QObject>


class InstagramRequestv2 : public QObject
{
    Q_OBJECT
public:
    explicit InstagramRequestv2(QObject *parent = 0);

    void request(QString endpoint, QByteArray post);
    void fileRquest(QString endpoint, QString boundary, QByteArray data);
    QString generateSignature(QJsonObject data);
    QString buildBody(QList<QList<QString> > bodies, QString boundary);

private:
    QString API_URL = "https://i.instagram.com/api/v1/";
    QString USER_AGENT = "Instagram 10.33.0 Android (18/4.3; 320dpi; 720x1280; Xiaomi; HM 1SW; armani; qcom; en_US)";
    QString IS_SIG_KEY = "0443b39a54b05f064a4917a3d1da4d6524a3fb0878eacabf1424515051674daa";
            //"b03e0daaf2ab17cda2a569cace938d639d1288a1197f9ecf97efd0a4ec0874d7";

    QString SIG_KEY_VERSION = "4";

    QDir m_data_path;

    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QNetworkCookieJar *m_jar;
    QList<QNetworkReply*> connections;

Q_SIGNALS:
    void replyStringReady(QVariant ans);

public Q_SLOTS:

private Q_SLOTS:
    void finishGetUrl();
    void saveCookie();
};
#endif // INSTAGRAMREQUESTV2_H
