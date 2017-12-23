#ifndef INSTAGRAMREQUESTV2_H
#define INSTAGRAMREQUESTV2_H

#include <QNetworkReply>
#include <QObject>

#include "instagramconstants.h"

class InstagramRequestv2 : public QObject
{
    Q_OBJECT
public:
    explicit InstagramRequestv2(QNetworkReply *reply, QObject *parent = 0);

    static QString generateSignature(QJsonObject data);
    QString buildBody(QList<QList<QString> > bodies, QString boundary);

private:
    QNetworkReply *m_reply;

Q_SIGNALS:
    void replyStringReady(QVariant ans);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

public Q_SLOTS:

private Q_SLOTS:
    void finishGetUrl();
};
#endif // INSTAGRAMREQUESTV2_H
