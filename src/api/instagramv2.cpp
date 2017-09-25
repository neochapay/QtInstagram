/*
 * BASED ON https://github.com/mgp25/Instagram-API
 */
#include "instagramv2.h"
#include "instagramrequest.h"

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
    this->m_data_path =  QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    if(!m_data_path.exists())
    {
        m_data_path.mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    QUuid uuid;
    this->m_uuid = uuid.createUuid().toString();

    this->m_device_id = this->generateDeviceId();

    this->setUser();
}

QString Instagramv2::generateDeviceId()
{
    QFileInfo fi(m_data_path.absolutePath());
    QByteArray volatile_seed = QString::number(fi.created().toMSecsSinceEpoch()).toUtf8();

    QByteArray data_1 = QCryptographicHash::hash(
                        QString(this->m_username+this->m_password).toUtf8(),
                        QCryptographicHash::Md5).toHex();

    QString data_2 = QString(QCryptographicHash::hash(
                QString(data_1+volatile_seed).toUtf8(),
                QCryptographicHash::Md5).toHex());

    QString data = "android-"+data_2.left(16);

    return data;
}


void Instagramv2::setUser()
{
    if(this->m_username.length() == 0 or this->m_password.length() == 0)
    {
        emit error("Username and/or password is clean");
    }
    else
    {
        QFile f_cookie(m_data_path.absolutePath()+"/cookies.dat");
        QFile f_userId(m_data_path.absolutePath()+"/userId.dat");
        QFile f_token(m_data_path.absolutePath()+"/token.dat");

        if(f_cookie.exists() && f_userId.exists() && f_token.exists())
        {
            this->m_isLoggedIn = true;
            this->m_username_id = f_userId.readAll().trimmed();
            this->m_rank_token = this->m_username_id+"_"+this->m_uuid;
            this->m_token = f_token.readAll().trimmed();

            this->doLogin();
        }
    }
}


void Instagramv2::login(bool forse)
{
    if(!this->m_isLoggedIn or forse)
    {
        this->setUser();
        InstagramRequest *loginRequest = new InstagramRequest();
        loginRequest->request("si/fetch_headers/?challenge_type=signup&guid="+this->m_uuid,NULL);
        QObject::connect(loginRequest,SIGNAL(replySrtingReady(QVariant)),this,SLOT(doLogin()));
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

    InstagramRequest *looutRequest = new InstagramRequest();
    looutRequest->request("accounts/logout/",NULL);
    QObject::connect(looutRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(doLogout(QVariant)));
}

void Instagramv2::doLogin()
{
    InstagramRequest *request = new InstagramRequest();
    QRegExp rx("token=(\\w+);");
    QFile f(m_data_path.absolutePath()+"/cookies.dat");
    if (!f.open(QFile::ReadOnly))
    {
        emit error("Can`t open token file");
    }
    QTextStream in(&f);
    rx.indexIn(in.readAll());
    if(rx.cap(1).length() > 0)
    {
        this->m_token = rx.cap(1);
    }
    else
    {
        emit error("Can`t find token");
    }
    QUuid uuid;

    QJsonObject data;
        data.insert("phone_id",     uuid.createUuid().toString());
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("username",     this->m_username);
        data.insert("guid",         this->m_uuid);
        data.insert("device_id",    this->m_device_id);
        data.insert("password",     this->m_password);
        data.insert("login_attempt_count", QString("0"));

    QString signature = request->generateSignature(data);
    request->request("accounts/login/",signature.toUtf8());

    QObject::connect(request,SIGNAL(replySrtingReady(QVariant)),this,SLOT(profileConnect(QVariant)));
}

void Instagramv2::profileConnect(QVariant profile)
{
    QJsonDocument profile_doc = QJsonDocument::fromJson(profile.toString().toUtf8());
    QJsonObject profile_obj = profile_doc.object();
    if(profile_obj["status"].toString().toUtf8() == "fail")
    {
        emit error(profile_obj["message"].toString().toUtf8());
        emit profileConnectedFail();
    }
    else
    {

        QJsonObject user = profile_obj["logged_in_user"].toObject();

        this->m_isLoggedIn = true;

        this->m_username_id = QString::number(user["pk"].toDouble(),'g', 10);

        this->m_rank_token = this->m_username_id+"_"+this->m_uuid;

        this->syncFeatures();

        emit profileConnected(profile);
    }
}


void Instagramv2::syncFeatures()
{
    InstagramRequest *syncRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("_uid",         this->m_username_id);
        data.insert("id",           this->m_username_id);
        data.insert("password",     this->m_password);
        data.insert("experiments",  EXPERIMENTS);

    QString signature = syncRequest->generateSignature(data);
    syncRequest->request("qe/sync/",signature.toUtf8());
}

//FIXME: uploadImage is not public yeat. Give me few weeks to optimize code
void Instagramv2::postImage(QString path, QString caption, QString upload_id)
{
    this->m_caption = caption;
    this->m_image_path = path;

    QFile image(path);
    if(!image.open(QIODevice::ReadOnly))
    {
        emit error("Image not found");
    }

    QByteArray dataStream = image.readAll();

    QFileInfo info(image.fileName());
    QString ext = info.completeSuffix();

    QString boundary = this->m_uuid;

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
    body += this->m_uuid.replace("{","").replace("}","")+"\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"_csrftoken\"\r\n\r\n";
    body += this->m_token+"\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"image_compression\"\r\n\r\n";
    body += "{\"lib_name\":\"jt\",\"lib_version\":\"1.3.0\",\"quality\":\"70\"}\r\n";

    body += "--"+boundary+"\r\n";
    body += "Content-Disposition: form-data; name=\"photo\"; filename=\"pending_media_"+upload_id+"."+ext+"\"\r\n";
    body += "Content-Transfer-Encoding: binary\r\n";
    body += "Content-Type: application/octet-stream\r\n\r\n";

    body += dataStream+"\r\n";
    body += "--"+boundary+"--";

    InstagramRequest *putPhotoReqest = new InstagramRequest();
    putPhotoReqest->fileRquest("upload/photo/",boundary, body);

    QObject::connect(putPhotoReqest,SIGNAL(replySrtingReady(QVariant)),this,SLOT(configurePhoto(QVariant)));
}

void Instagramv2::configurePhoto(QVariant answer)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toByteArray());
    QJsonObject jsonObject = jsonResponse.object();
    if(jsonObject["status"].toString() != QString("ok"))
    {
        emit error(jsonObject["message"].toString());
    }
    else
    {
        QString upload_id = jsonObject["upload_id"].toString();
        if(upload_id.length() == 0)
        {
            emit error("Wrong UPLOAD_ID:"+upload_id);
        }
        else
        {
            QImage image = QImage(this->m_image_path);
            InstagramRequest *configureImageRequest = new InstagramRequest();

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
                data.insert("caption",              this->m_caption);
                data.insert("_uuid",                this->m_uuid);
                data.insert("_uid",                 this->m_username_id);
                data.insert("_csrftoken",           "Set-Cookie: csrftoken="+this->m_token);

            QString signature = configureImageRequest->generateSignature(data);
            configureImageRequest->request("media/configure/",signature.toUtf8());
            QObject::connect(configureImageRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(imageConfigureDataReady(QVariant)));
        }
    }
    this->m_caption = "";
    this->m_image_path = "";
}

//FIXME: uploadImage is not public yeat. Give me few weeks to optimize code
void Instagramv2::postVideo(QFile *video)
{

}

/*void Instagramv2::editMedia(QString mediaId, QString captionText)
{
    InstagramRequest *editMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("caption_text", captionText);

    QString signature = editMediaRequest->generateSignature(data);
    editMediaRequest->request("media/"+mediaId+"/edit_media/",signature.toUtf8());
    QObject::connect(editMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaEdited(QVariant)));
}

void Instagramv2::infoMedia(QString mediaId)
{
    InstagramRequest *infoMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id", mediaId);

    QString signature = infoMediaRequest->generateSignature(data);
    infoMediaRequest->request("media/"+mediaId+"/info/",signature.toUtf8());
    QObject::connect(infoMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaInfoReady(QVariant)));
}

void Instagramv2::deleteMedia(QString mediaId)
{
    InstagramRequest *deleteMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",    mediaId);

    QString signature = deleteMediaRequest->generateSignature(data);
    deleteMediaRequest->request("media/"+mediaId+"/delete/",signature.toUtf8());
    QObject::connect(deleteMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaDeleted(QVariant)));
}*/

/*void Instagramv2::removeSelftag(QString mediaId)
{
    InstagramRequest *removeSelftagRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = removeSelftagRequest->generateSignature(data);
    removeSelftagRequest->request("usertags/"+mediaId+"/remove/",signature.toUtf8());
    QObject::connect(removeSelftagRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(removeSelftagDone(QVariant)));
}*/

/*void Instagramv2::comment(QString mediaId, QString commentText)
{
    InstagramRequest *postCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("comment_text", commentText);

    QString signature = postCommentRequest->generateSignature(data);
    postCommentRequest->request("media/"+mediaId+"/comment/",signature.toUtf8());
    QObject::connect(postCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentPosted(QVariant)));
}

void Instagramv2::deleteComment(QString mediaId, QString commentId, QString captionText)
{
    InstagramRequest *deleteCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("caption_text", captionText);

    QString signature = deleteCommentRequest->generateSignature(data);
    deleteCommentRequest->request("media/"+mediaId+"/comment/"+commentId+"/delete/",signature.toUtf8());
    QObject::connect(deleteCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentDeleted(QVariant)));
}*/

//FIXME changeProfilePicture is not public yeat. Give me few weeks to optimize code
//void Instagramv2::changeProfilePicture(QFile *photo)
//{

//}

/*void Instagramv2::removeProfilePicture()
{
    InstagramRequest *removeProfilePictureRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   this->m_token);

    QString signature = removeProfilePictureRequest->generateSignature(data);
    removeProfilePictureRequest->request("accounts/remove_profile_picture/",signature.toUtf8());
    QObject::connect(removeProfilePictureRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(profilePictureDeleted(QVariant)));
}*/

/*void Instagramv2::setPrivateAccount()
{
    InstagramRequest *setPrivateRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = setPrivateRequest->generateSignature(data);
    setPrivateRequest->request("accounts/set_private/",signature.toUtf8());
    QObject::connect(setPrivateRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(setProfilePrivate(QVariant)));
}

void Instagramv2::setPublicAccount()
{
    InstagramRequest *setPublicRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = setPublicRequest->generateSignature(data);
    setPublicRequest->request("accounts/set_public/",signature.toUtf8());
    QObject::connect(setPublicRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(setProfilePublic(QVariant)));
}*/

/*void Instagramv2::getProfileData()
{
    InstagramRequest *getProfileRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = getProfileRequest->generateSignature(data);
    getProfileRequest->request("accounts/current_user/?edit=true",signature.toUtf8());
    QObject::connect(getProfileRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(profileDataReady(QVariant)));
} */
/**
 * Edit profile.
 *
 * @param QString url
 *   Url - website. "" for nothing
 * @param QString phone
 *   Phone number. "" for nothing
 * @param QString first_name
 *   Name. "" for nothing
 * @param QString email
 *   Email. Required.
 * @param bool gender
 *   Gender. male = true , female = false
 */
/*void Instagramv2::editProfile(QString url, QString phone, QString first_name, QString biography, QString email, bool gender)
{
    InstagramRequest *editProfileRequest = new InstagramRequest();
    QString gen_string;
    if(gender)
    {
        gen_string = "1";
    }
    else
    {
        gen_string = "0";
    }

    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("external_url", url);
        data.insert("phone_number", phone);
        data.insert("username",     this->m_username);
        data.insert("full_name",    first_name);
        data.insert("biography",    biography);
        data.insert("email",        email);
        data.insert("gender",       gen_string);

    QString signature = editProfileRequest->generateSignature(data);
    editProfileRequest->request("accounts/edit_profile/",signature.toUtf8());
    QObject::connect(editProfileRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(editDataReady(QVariant)));
}*/

/*void Instagramv2::getUsernameInfo(QString usernameId)
{
    InstagramRequest *getUsernameRequest = new InstagramRequest();
    getUsernameRequest->request("users/"+usernameId+"/info/",NULL);
    QObject::connect(getUsernameRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(usernameDataReady(QVariant)));
}*/

/*void Instagramv2::getRecentActivity()
{
    InstagramRequest *getRecentActivityRequest = new InstagramRequest();
    getRecentActivityRequest->request("news/inbox/?",NULL);
    QObject::connect(getRecentActivityRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(recentActivityDataReady(QVariant)));
}*/

/*void Instagramv2::getFollowingRecentActivity()
{
    InstagramRequest *getFollowingRecentRequest = new InstagramRequest();
    wingRecentRequest->request("news/?",NULL);
    QObject::connect(getFollowingRecentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followingRecentDataReady(QVariant)));
}*/

/*void Instagramv2::getUserTags(QString usernameId)
{
    InstagramRequest *getUserTagsRequest = new InstagramRequest();
    getUserTagsRequest->request("usertags/"+usernameId+"/feed/?rank_token="+this->m_rank_token+"&ranked_content=true&",NULL);
    QObject::connect(getUserTagsRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userTagsDataReady(QVariant)));
}*/

/*void Instagramv2::tagFeed(QString tag)
{
    InstagramRequest *getTagFeedRequest = new InstagramRequest();
    getTagFeedRequest->request("feed/tag/"+tag+"/?rank_token="+this->m_rank_token+"&ranked_content=true&",NULL);
    QObject::connect(getTagFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(tagFeedDataReady(QVariant)));
}*/

/*void Instagramv2::userFeed(QString user)
{
    InstagramRequest *getUserFeedRequest = new InstagramRequest();
    getUserFeedRequest->request("users/search/?query="+user+"&is_typeahead=true&rank_token="+this->m_rank_token+"&",NULL);
    QObject::connect(getUserFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userFeedDataReady(QVariant)));
}*/

/*void Instagramv2::exploreFeed(QString session_id)
{
    InstagramRequest *getExploreRequest = new InstagramRequest();
    getExploreRequest->request("discover/explore/?is_prefetch=false&is_from_promote=false&session_id=" + this->m_token +
                               "&module=explore_popular"
                               ,NULL);
    QObject::connect(getExploreRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(exploreDataReady(QVariant)));
} */

/*void Instagramv2::storiesFeed(QString session_id)
{
    InstagramRequest *getStoriesRequest = new InstagramRequest();
    getStoriesRequest->request("feed/reels_tray/?"
                               ,NULL);
    QObject::connect(getStoriesRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(storiesDataReady(QVariant)));
}*/


/*void Instagramv2::getTimeLine(QString max_id)
{
    QString target ="feed/timeline/?rank_token="+this->m_rank_token+"&ranked_content=true&";

    if(max_id.length() > 0)
    {
        target += "&max_id="+max_id;
    }

    InstagramRequest *getTimeLineRequest = new InstagramRequest();
    getTimeLineRequest->request(target,NULL);
    QObject::connect(getTimeLineRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(timeLineDataReady(QVariant)));
}*/

/*void Instagramv2::getUserFeed(QString usernameID, QString maxid, QString minTimestamp)
{
    QString endpoint;
    endpoint = "feed/user/"+usernameID+"/?rank_token="+this->m_rank_token;
    if(maxid.length() > 0)
    {
        endpoint += "&max_id="+maxid;
    }
    if(minTimestamp.length() > 0)
    {
        endpoint += "&min_timestamp="+minTimestamp;
    }
    endpoint += "&ranked_content=true";

    InstagramRequest *getUserTimeLineRequest = new InstagramRequest();
    getUserTimeLineRequest->request(endpoint,NULL);
    QObject::connect(getUserTimeLineRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userTimeLineDataReady(QVariant)));
} */

void Instagramv2::getPopularFeed()
{
    InstagramRequest *getPopularFeedRequest = new InstagramRequest();
    getPopularFeedRequest->request("feed/popular/?people_teaser_supported=1&rank_token="+this->m_rank_token+"&ranked_content=true&",NULL);
    QObject::connect(getPopularFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(popularFeedDataReady(QVariant)));

}

/*void Instagramv2::getMediaLikers(QString mediaId)
{
    InstagramRequest *getMediaLikersRequest = new InstagramRequest();
    getMediaLikersRequest->request("media/"+mediaId+"/likers/?",NULL);
    QObject::connect(getMediaLikersRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaLikersDataReady(QVariant)));
}*/

/*void Instagramv2::like(QString mediaId)
{
    InstagramRequest *likeRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",     mediaId);

    QString signature = likeRequest->generateSignature(data);
    likeRequest->request("media/"+mediaId+"/like/",signature.toUtf8());
    QObject::connect(likeRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likeDataReady(QVariant)));
}

void Instagramv2::unLike(QString mediaId)
{
    InstagramRequest *unLikeRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",     mediaId);

    QString signature = unLikeRequest->generateSignature(data);
    unLikeRequest->request("media/"+mediaId+"/unlike/",signature.toUtf8());
    QObject::connect(unLikeRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unLikeDataReady(QVariant)));
}*/


/*void Instagramv2::likeComment(QString commentId)
{
    InstagramRequest *likeCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = likeCommentRequest->generateSignature(data);
    likeCommentRequest->request("media/"+commentId+"/like/",signature.toUtf8());
    QObject::connect(likeCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likeCommentDataReady(QVariant)));
}

void Instagramv2::unlikeComment(QString commentId)
{
    InstagramRequest *unLikeCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = unLikeCommentRequest->generateSignature(data);
    unLikeCommentRequest->request("media/"+commentId+"/like/",signature.toUtf8());
    QObject::connect(unLikeCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unLikeCommentDataReady(QVariant)));
}*/

/*void Instagramv2::getMediaComments(QString mediaId)
{
    InstagramRequest *getMediaCommentsRequest = new InstagramRequest();
    getMediaCommentsRequest->request("media/"+mediaId+"/comments/?",NULL);
    QObject::connect(getMediaCommentsRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaCommentsDataReady(QVariant)));
}*/

/*void Instagramv2::follow(QString userId)
{
    InstagramRequest *followRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("user_id",     userId);

    QString signature = followRequest->generateSignature(data);
    followRequest->request("friendships/create/"+userId+"/",signature.toUtf8());
    QObject::connect(followRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followDataReady(QVariant)));
}

void Instagramv2::unFollow(QString userId)
{
    InstagramRequest *unFollowRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("user_id",     userId);

    QString signature = unFollowRequest->generateSignature(data);
    unFollowRequest->request("friendships/destroy/"+userId+"/",signature.toUtf8());
    QObject::connect(unFollowRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unFollowDataReady(QVariant)));
}*/

/*void Instagramv2::block(QString userId)
{
    InstagramRequest *blockRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("user_id",     userId);

    QString signature = blockRequest->generateSignature(data);
    blockRequest->request("friendships/block/"+userId+"/",signature.toUtf8());
    QObject::connect(blockRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(blockDataReady(QVariant)));
}

void Instagramv2::unBlock(QString userId)
{
    InstagramRequest *unBlockRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("user_id",     userId);

    QString signature = unBlockRequest->generateSignature(data);
    unBlockRequest->request("friendships/unblock/"+userId+"/",signature.toUtf8());
    QObject::connect(unBlockRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unBlockDataReady(QVariant)));
} */

/*void Instagramv2::getFriendship(QString userId)
{
    InstagramRequest *userFriendshipRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("user_id",     userId);

    QString signature = userFriendshipRequest->generateSignature(data);
    userFriendshipRequest->request("friendships/show/"+userId+"/",signature.toUtf8());
    QObject::connect(userFriendshipRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userFriendshipDataReady(QVariant)));
}*/

/*void Instagramv2::getLikedMedia(QString maxid)
{
    InstagramRequest *getLikedMediaRequest = new InstagramRequest();
    if(maxid.length() == 0)
    {
        getLikedMediaRequest->request("feed/liked/?",NULL);
    }
    else
    {
        getLikedMediaRequest->request("feed/liked/?max_id="+maxid,NULL);
    }
    QObject::connect(getLikedMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likedMediaDataReady(QVariant)));
}*/

/*void Instagramv2::getFollowing(QString userId, QString max_id)
{
    InstagramRequest *getUserFollowingsRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    if(maxId != ""){
        data.insert("max_id",     maxId);
    }

    QString signature = getUserFollowingsRequest->generateSignature(data);
    getUserFollowingsRequest->request("friendships/"+userId+"/following/",signature.toUtf8());
    QObject::connect(getUserFollowingsRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userFollowingsDataReady(QVariant)));
}

void Instagramv2::getFollowers(QString userId, QString max_id)
{
    qDebug() << userId;
    InstagramRequest *getUserFollowersRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    if(maxId != ""){
        data.insert("max_id",     maxId);
    }

    QString signature = getUserFollowersRequest->generateSignature(data);
    getUserFollowersRequest->request("friendships/"+userId+"/followers/",signature.toUtf8());
    QObject::connect(getUserFollowersRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userFollowersDataReady(QVariant)));
} */

/*
 * Return json string
 * {
 *   "username":    STRING  Checking username,
 *   "available":   BOOL    Aviable to registration,
 *   "status":      STRING  Status of request,
 *   "error":       STRING  Error string if aviable
 *   }
 */
/*void Instagramv2::checkUsername(QString username)
{
    InstagramRequest *checkUsernameRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_csrftoken",   QString("missing"));
        data.insert("username",     username);

    QString signature = checkUsernameRequest->generateSignature(data);
    checkUsernameRequest->request("users/check_username/",signature.toUtf8());
    QObject::connect(checkUsernameRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(usernameCheckDataReady(QVariant)));
}*/


/*
 * Return JSON string
 * {
 *  "status": STRING    Status of request,
 *  "errors":{
 *            ARRAY     Array of errors if aviable
 *      "password":[],  STRING  Error message if password wrong if aviable
 *      "email":[],     STRING  Error message if email wrong if aviable
 *      "FIELD_ID":[]   STRING  Error message if FIELD_ID wrong if aviable
 *  },
 *  "account_created",  BOOL Status of creation account
 *  "created_user"      ARRAY Array of new user params
 *  }
 *
 */
/*void Instagramv2::createAccount(QString username, QString password, QString email)
{
    InstagramRequest *createAccountRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",               this->m_uuid);
        data.insert("_csrftoken",          QString("missing"));
        data.insert("username",            username);
        data.insert("first_name",          QString(""));
        data.insert("guid",                this->m_uuid);
        data.insert("device_id",           this->m_device_id);
        data.insert("email",               email);
        data.insert("force_sign_up_code",  QString(""));
        data.insert("qs_stamp",            QString(""));
        data.insert("password",            password);

    QString signature = createAccountRequest->generateSignature(data);
    createAccountRequest->request("accounts/create/",signature.toUtf8());
    QObject::connect(createAccountRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(createAccountDataReady(QVariant)));
}*/

void Instagramv2::searchUsername(QString username)
{
    InstagramRequest *searchUsernameRequest = new InstagramRequest();
    searchUsernameRequest->request("users/"+username+"/usernameinfo/", NULL);
    QObject::connect(searchUsernameRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(searchUsernameDataReady(QVariant)));
}

/*void Instagramv2::getInbox()
{
    InstagramRequest *getInboxRequest = new InstagramRequest();
    getInboxRequest->request("direct_v2/inbox/", NULL);
    QObject::connect(getInboxRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(getInboxDataReady(QVariant)));
}

void Instagramv2::getDirectThread(QString threadId)
{
    InstagramRequest *getDirectThreadRequest = new InstagramRequest();
    getDirectThreadRequest->request("direct_v2/threads/"+threadId+"/", NULL);
    QObject::connect(getDirectThreadRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(getDirectThreadDataReady(QVariant)));
}

void Instagramv2::getPendingInbox()
{
    InstagramRequest *getPendingInboxRequest = new InstagramRequest();
    getPendingInboxRequest->request("direct_v2/pending_inbox/", NULL);
    QObject::connect(getPendingInboxRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(getPendingInboxDataReady(QVariant)));
}

void Instagramv2::getRecentRecipients()
{
    InstagramRequest *getRecentRecipientsRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_csrftoken",   QString("missing"));
        data.insert("show_threads", QString("true"));

    QString signature = getRecentRecipientsRequest->generateSignature(data);
    getRecentRecipientsRequest->request("direct_share/recent_recipients/", signature.toUtf8());
    QObject::connect(getRecentRecipientsRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(getRecentRecipientsDataReady(QVariant)));

}*/

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
