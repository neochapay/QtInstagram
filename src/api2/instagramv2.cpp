/*
 * BASED ON https://github.com/mgp25/Instagram-API
 */
#include "instagramv2.h"
//#include "instagramrequestv2.h"

#include <QCryptographicHash>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QImage>
#include <QDataStream>
#include <QDebug>

Instagramv2::Instagramv2(QObject *parent)
    : QObject(parent)
{
    m_data_path =  QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    if(!m_data_path.exists())
    {
        m_data_path.mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    QUuid uuid;
    m_uuid = uuid.createUuid().toString();

    m_device_id = generateDeviceId();
    requestGlobal = new InstagramRequestv2();
    setUser();
}

QString Instagramv2::generateDeviceId()
{
    QFileInfo fi(m_data_path.absolutePath());
    QByteArray volatile_seed = QString::number(fi.created().toMSecsSinceEpoch()).toUtf8();

    QByteArray data_1 = QCryptographicHash::hash(
                        QString(m_username+m_password).toUtf8(),
                        QCryptographicHash::Md5).toHex();

    QString data_2 = QString(QCryptographicHash::hash(
                QString(data_1+volatile_seed).toUtf8(),
                QCryptographicHash::Md5).toHex());

    QString data = "android-"+data_2.left(16);

    return data;
}

void Instagramv2::setUser()
{
    if(m_username.length() == 0 || m_password.length() == 0)
    {
        Q_EMIT error("Username and/or password is clean");
    }
    else
    {
        QFile f_cookie(m_data_path.absolutePath()+"/cookies.dat");
        QFile f_userId(m_data_path.absolutePath()+"/userId.dat");
        QFile f_token(m_data_path.absolutePath()+"/token.dat");

        if(f_cookie.exists() && f_userId.exists() && f_token.exists())
        {
            m_isLoggedIn = true;
            m_username_id = f_userId.readAll().trimmed();
            m_rank_token = m_username_id+"_"+m_uuid;
            m_token = f_token.readAll().trimmed();
            doLogin();
        }
    }
}

void Instagramv2::login(bool forse)
{
    if(!m_isLoggedIn or forse)
    {
        setUser();
        InstagramRequestv2 *loginRequest = new InstagramRequestv2();
        loginRequest->request("si/fetch_headers/?challenge_type=signup&guid="+m_uuid,NULL);
        QObject::connect(loginRequest,&InstagramRequestv2::replyStringReady,this,&Instagramv2::doLogin);
    }
}

void Instagramv2::logout()
{
    QFile f_cookie(m_data_path.absolutePath()+"/cookies.dat");
    QFile f_userId(m_data_path.absolutePath()+"/userId.dat");
    QFile f_token(m_data_path.absolutePath()+"/token.dat");

    f_cookie.remove();
    f_userId.remove();
    f_token.remove();

    InstagramRequestv2 *looutRequest = new InstagramRequestv2();
    looutRequest->request("accounts/logout/",NULL);
    QObject::connect(looutRequest,&InstagramRequestv2::replyStringReady,this,&Instagramv2::doLogout);
}

void Instagramv2::doLogin()
{
    InstagramRequestv2 *request = new InstagramRequestv2();
    QRegExp rx("token=(\\w+);");
    QFile f(m_data_path.absolutePath()+"/cookies.dat");
    if (!f.open(QFile::ReadOnly))
    {
        Q_EMIT error("Can`t open token file");
    }
    QTextStream in(&f);
    rx.indexIn(in.readAll());
    if(rx.cap(1).length() > 0)
    {
        m_token = rx.cap(1);
    }
    else
    {
        Q_EMIT error("Can`t find token");
    }
    QUuid uuid;

    QJsonObject data;
        data.insert("phone_id",     uuid.createUuid().toString());
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("username",     m_username);
        data.insert("guid",         m_uuid);
        data.insert("device_id",    m_device_id);
        data.insert("password",     m_password);
        data.insert("login_attempt_count", QString("0"));

    QString signature = request->generateSignature(data);
    request->request("accounts/login/",signature.toUtf8());

    QObject::connect(request,&InstagramRequestv2::replyStringReady,this,&Instagramv2::profileConnect);
}

void Instagramv2::profileConnect(QVariant profile)
{
    QJsonDocument profile_doc = QJsonDocument::fromJson(profile.toString().toUtf8());
    QJsonObject profile_obj = profile_doc.object();
    if(profile_obj["status"].toString().toUtf8() == "fail")
    {
        Q_EMIT error(profile_obj["message"].toString().toUtf8());
        Q_EMIT profileConnectedFail();
    }
    else
    {
        QJsonObject user = profile_obj["logged_in_user"].toObject();
        m_isLoggedIn = true;
        m_username_id = QString::number(user["pk"].toDouble(),'g', 10);

        m_rank_token = m_username_id+"_"+m_uuid;

        syncFeatures();

        Q_EMIT profileConnected(profile);
    }
}

void Instagramv2::syncFeatures()
{
    InstagramRequestv2 *syncRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("_uid",         m_username_id);
        data.insert("id",           m_username_id);
        data.insert("password",     m_password);
        data.insert("experiments",  EXPERIMENTS);

    QString signature = syncRequest->generateSignature(data);
    syncRequest->request("qe/sync/",signature.toUtf8());
}

//FIXME: uploadImage is not public yeat. Give me few weeks to optimize code
void Instagramv2::postImage(QString path, QString caption, QString upload_id)
{
    m_caption = caption;
    m_image_path = path;

    QFile image(path);
    if(!image.open(QIODevice::ReadOnly))
    {
        Q_EMIT error("Image not found");
    }

    QByteArray dataStream = image.readAll();

    QFileInfo info(image.fileName());
    QString ext = info.completeSuffix();

    QString boundary = m_uuid;

    if(upload_id.size() == 0)
    {
        upload_id =QString::number(QDateTime::currentMSecsSinceEpoch());
    }
/*Body build*/
    QByteArray body = "";
    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"upload_id\"\r\n\r\n";
    body += upload_id+"\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"_uuid\"\r\n\r\n";
    body += m_uuid.replace("{","").replace("}","")+"\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"_csrftoken\"\r\n\r\n";
    body += m_token+"\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"image_compression\"\r\n\r\n";
    body += "{\"lib_name\":\"jt\",\"lib_version\":\"1.3.0\",\"quality\":\"70\"}\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"photo\"; filename=\"pending_media_"+upload_id+"."+ext+"\"\r\n";
    body += "Content-Transfer-Encoding: binary\r\n";
    body += "Content-Type: application/octet-stream\r\n\r\n";

    body += dataStream+"\r\n";
    body += "--"+boundary+"--";

    InstagramRequestv2 *putPhotoReqest = new InstagramRequestv2();
    putPhotoReqest->fileRquest("upload/photo/",boundary, body);

    QObject::connect(putPhotoReqest,&InstagramRequestv2::replyStringReady,this,&Instagramv2::configurePhoto);
}

void Instagramv2::configurePhoto(QVariant answer)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toByteArray());
    QJsonObject jsonObject = jsonResponse.object();
    if(jsonObject["status"].toString() != QString("ok"))
    {
        Q_EMIT error(jsonObject["message"].toString());
    }
    else
    {
        QString upload_id = jsonObject["upload_id"].toString();
        if(upload_id.length() == 0)
        {
            Q_EMIT error("Wrong UPLOAD_ID:"+upload_id);
        }
        else
        {
            QImage image = QImage(m_image_path);
            InstagramRequestv2 *configureImageRequest = new InstagramRequestv2();

            QJsonObject device;
                device.insert("manufacturer",   QString("Xiaomi"));
                device.insert("model",          QString("HM 1SW"));
                device.insert("android_version",18);
                device.insert("android_release",QString("4.3"));
            QJsonObject extra;
                extra.insert("source_width",    image.width());
                extra.insert("source_height",   image.height());

            QJsonArray crop_original_size;
                crop_original_size.append(image.width());
                crop_original_size.append(image.height());
            QJsonArray crop_center;
                crop_center.append(0.0);
                crop_center.append(-0.0);

            QJsonObject edits;
                edits.insert("crop_original_size", crop_original_size);
                edits.insert("crop_zoom",          1.3333334);
                edits.insert("crop_center",        crop_center);

            QJsonObject data;
                data.insert("upload_id",            upload_id);
                data.insert("camera_model",         QString("HM1S"));
                data.insert("source_type",          3);
                data.insert("date_time_original",   QDateTime::currentDateTime().toString("yyyy:MM:dd HH:mm:ss"));
                data.insert("camera_make",          QString("XIAOMI"));
                data.insert("edits",                edits);
                data.insert("extra",                extra);
                data.insert("device",               device);
                data.insert("caption",              m_caption);
                data.insert("_uuid",                m_uuid);
                data.insert("_uid",                 m_username_id);
                data.insert("_csrftoken",           "Set-Cookie: csrftoken="+m_token);

            QString signature = configureImageRequest->generateSignature(data);
            configureImageRequest->request("media/configure/",signature.toUtf8());
            QObject::connect(configureImageRequest,&InstagramRequestv2::replyStringReady,this,&Instagramv2::imageConfigureDataReady);
        }
    }
    m_caption = "";
    m_image_path = "";
}

//FIXME: uploadImage is not public yeat. Give me few weeks to optimize code
void Instagramv2::postVideo(QFile *video)
{

}

void Instagramv2::getPopularFeed()
{
    InstagramRequestv2 *getPopularFeedRequest = new InstagramRequestv2();
    getPopularFeedRequest->request("feed/popular/?people_teaser_supported=1&rank_token="+m_rank_token+"&ranked_content=true&",NULL);
    QObject::connect(getPopularFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(popularFeedDataReady(QVariant)));

}

void Instagramv2::searchUsername(QString username)
{
    InstagramRequestv2 *searchUsernameRequest = new InstagramRequestv2();
    searchUsernameRequest->request("users/"+username+"/usernameinfo/", NULL);
    QObject::connect(searchUsernameRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(searchUsernameDataReady(QVariant)));
}

void Instagramv2::rotateImg(QString filename, qreal deg)
{
    QImage image(filename);
    QTransform rot;
    rot.rotate(deg);
    image = image.transformed(rot);

    QFile imgFile(filename);
    imgFile.open(QIODevice::ReadWrite);

    if(!image.save(&imgFile,"JPG",100))
    {
        qDebug() << "NOT SAVE";
    }

    imgFile.close();
}

void Instagramv2::cropImg(QString filename, bool squared)
{

    QImage image(filename);
    QTransform rot;
    rot.rotate(90);
    image = image.transformed(rot);

    int min_size = qMin(image.width(),image.height());
    int max_size = qMax(image.width(),image.height());

    if(squared)
    {
        image = image.copy(0,(max_size-min_size)/2,min_size,min_size);
    }
    else
    {
        int size54 = min_size*5/4;
        image = image.copy(0,(max_size-size54)/2,min_size,size54);
    }

    QFile imgFile(filename);
    imgFile.open(QIODevice::ReadWrite);

    if(!image.save(&imgFile,"JPG",100))
    {
        qDebug() << "NOT SAVE";
    }

    imgFile.close();
}

void Instagramv2::cropImg(QString in_filename, QString out_filename, int topSpace, bool squared)
{
    QImage image(in_filename);
    int min_size = qMin(image.width(),image.height());

    if(squared)
    {
        image = image.copy(0,topSpace,min_size,min_size);
    }
    else
    {
        int size54 = min_size*5/4;
        image = image.copy(0,topSpace,min_size,size54);
    }

    if(!image.save(out_filename))
    {
        qDebug() << "NOT SAVE HERE";
    }
}
