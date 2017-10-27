#ifndef FAKE_NETWORK_H
#define FAKE_NETWORK_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQmlNetworkAccessManagerFactory>

class FakeReply: public QNetworkReply
{
    Q_OBJECT
public:
    FakeReply(const QNetworkRequest &req, QIODevice *outgoingData,
              QNetworkAccessManager *parent):
        QNetworkReply(parent),
        m_offset(0)
    {
        m_url = req.url();
        for (const QByteArray &header: req.rawHeaderList()) {
            m_headers.insert(QString::fromLatin1(header),
                             QString::fromLatin1(req.rawHeader(header)));
        }
        if (outgoingData) {
            m_body = outgoingData->readAll();
        }
        setRequest(req);
        QNetworkReply::open(QIODevice::ReadOnly);
    }

    QUrl url() const { return m_url; }
    QJsonObject headers() const { return m_headers; }
    QByteArray body() const { return m_body; }
    void setCookie(const QByteArray &header) {
        QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(header);
        manager()->cookieJar()->setCookiesFromUrl(cookies, m_url);
    }
    void setData(const QByteArray &data) {
        m_data = data;
        setFinished(true);
        QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
    }

    void abort() override {}
    qint64 readData(char *data, qint64 maxSize) override {
        qint64 count = qMin(maxSize, qint64(m_data.count() - m_offset));
        if (count < 0) return 0;
        memcpy(data, m_data.constData() + m_offset, count);
        m_offset += count;
        return count;
    }
private:
    friend class FakeNam;
    QUrl m_url;
    QJsonObject m_headers;
    QByteArray m_body;
    QByteArray m_data;
    qint64 m_offset;
};

class FakeNam: public QNetworkAccessManager
{
    Q_OBJECT
    using QNetworkAccessManager::QNetworkAccessManager;

    QNetworkReply *createRequest(Operation op, const QNetworkRequest &req,
                                 QIODevice *outgoingData) override {
        Q_UNUSED(op);
        FakeReply *reply = new FakeReply(req, outgoingData, this);
        Q_EMIT requestCreated(reply);
        return reply;
    }

Q_SIGNALS:
    void requestCreated(FakeReply *reply);
};

class FakeNamFactory: public QObject, public QQmlNetworkAccessManagerFactory
{
    Q_OBJECT
public:
    FakeNamFactory() {}
    QNetworkAccessManager *create(QObject *parent) override {
        FakeNam *nam = new FakeNam(parent);
        Q_EMIT namCreated(nam);
        return nam;
    }
Q_SIGNALS:
    void namCreated(FakeNam *nam);
};

#endif // FAKE_NETWORK_H
