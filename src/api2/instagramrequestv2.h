#ifndef INSTAGRAMREQUESTV2_H
#define INSTAGRAMREQUESTV2_H

#include <QDir>
#include <QNetworkAccessManager>
#include <QObject>

#include "instagramconstants.h"

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
