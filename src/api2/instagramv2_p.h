#ifndef QTINSTAGRAM_INSTAGRAMV2_P
#define QTINSTAGRAM_INSTAGRAMV2_P

#include "instagramv2.h"
#include "instagramrequestv2.h"

#include <QDir>
#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkCookieJar;

class Instagramv2Private: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Instagramv2)

public:
    Instagramv2Private(Instagramv2 *q);

    void loadCookies();

    InstagramRequestv2 *fileRequest(QString endpoint, QString boundary, QByteArray data);
    InstagramRequestv2 *request(QString endpoint, QByteArray post);

private Q_SLOTS:
    void setUser();
    void doLogin();
    void syncFeatures();
    void profileConnect(QVariant profile);
    void configurePhoto(QVariant answer);
    void saveCookie() const;

private:
    QString m_username;
    QString m_password;
    QString m_userID;
    QString m_debug;
    QString m_username_id;
    QString m_uuid;
    QString m_device_id;
    QString m_token;
    QString m_csrftoken=m_token;
    QString m_rank_token;
    QString m_IGDataPath;

    QString m_caption;
    QString m_image_path;

    QDir m_data_path;
    QNetworkAccessManager *m_manager;
    QNetworkCookieJar *m_jar;

    bool m_isLoggedIn = false;

    QString generateDeviceId();
    Instagramv2 *q_ptr;
};

#endif // QTINSTAGRAM_INSTAGRAMV2_P
