#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>

void Instagramv2::like(QString mediaId, QString module)
{
    InstagramRequest *likeRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = likeRequest->generateSignature(data);
    likeRequest->request("media/"+mediaId+"/like/",signature.toUtf8());
    QObject::connect(likeRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likeDataReady(QVariant)));
}

void Instagramv2::unLike(QString mediaId, QString module)
{
    InstagramRequest *unLikeRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = unLikeRequest->generateSignature(data);
    unLikeRequest->request("media/"+mediaId+"/unlike/",signature.toUtf8());
    QObject::connect(unLikeRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unLikeDataReady(QVariant)));
}

void Instagramv2::getLikedFeed(QString max_id)
{
    InstagramRequest *getLikedFeedRequest = new InstagramRequest();
    getLikedFeedRequest->request("feed/liked/"
                                 + (max_id.length()>0 ? "?max_id="+max_id : "")
                                 ,NULL);
    QObject::connect(getLikedFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likedFeedDataReady(QVariant)));
}

void Instagramv2::getInfoMedia(QString mediaId)
{
    InstagramRequest *infoMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id", mediaId);

    QString signature = infoMediaRequest->generateSignature(data);
    infoMediaRequest->request("media/"+mediaId+"/info/"
                              ,signature.toUtf8());
    QObject::connect(infoMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaInfoReady(QVariant)));
}

void Instagramv2::deleteMedia(QString mediaId, QString mediaType)
{
    InstagramRequest *deleteMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("media_id",    mediaId);

    QString signature = deleteMediaRequest->generateSignature(data);
    deleteMediaRequest->request("media/"+mediaId+"/delete/?"
                                "media_type=" + mediaType
                                ,signature.toUtf8());
    QObject::connect(deleteMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaDeleted(QVariant)));
}


void Instagramv2::editMedia(QString mediaId, QString captionText, QString mediaType)
{
    InstagramRequest *editMediaRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("caption_text", captionText);

    QString signature = editMediaRequest->generateSignature(data);
    editMediaRequest->request("media/"+mediaId+"/edit_media/"
                              ,signature.toUtf8());
    QObject::connect(editMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaEdited(QVariant)));
}

void Instagramv2::comment(QString mediaId, QString commentText, QString replyCommentId ,QString module)
{
    InstagramRequest *postCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        data.insert("comment_text", commentText);
        data.insert("containermoudle", module);
        data.insert("idempotence_token", postCommentRequest->generateUUID(true));
        data.insert("radio-type",   "wifi-none");
    if(replyCommentId != "" && replyCommentId.at(0) == '@')
        data.insert("replied_to_comment_id", replyCommentId);

    QString signature = postCommentRequest->generateSignature(data);
    postCommentRequest->request("media/"+mediaId+"/comment/",signature.toUtf8());
    QObject::connect(postCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentPosted(QVariant)));
}

void Instagramv2::deleteComment(QString mediaId, QString commentId)
{
    InstagramRequest *deleteCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
        //data.insert("caption_text", captionText);

    QString signature = deleteCommentRequest->generateSignature(data);
    deleteCommentRequest->request("media/"+mediaId+"/comment/"+commentId+"/delete/",signature.toUtf8());
    QObject::connect(deleteCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentDeleted(QVariant)));
}

void Instagramv2::likeComment(QString commentId)
{
    InstagramRequest *likeCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = likeCommentRequest->generateSignature(data);
    likeCommentRequest->request("media/"+commentId+"/comment_like/",signature.toUtf8());
    QObject::connect(likeCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentLiked(QVariant)));
}

void Instagramv2::unlikeComment(QString commentId)
{
    InstagramRequest *unlikeCommentRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = unlikeCommentRequest->generateSignature(data);
    unlikeCommentRequest->request("media/"+commentId+"/comment_unlike/",signature.toUtf8());
    QObject::connect(unlikeCommentRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(commentUnliked(QVariant)));
}

void Instagramv2::getComments(QString mediaId, QString max_id)
{
    InstagramRequest *getMediaCommentsRequest = new InstagramRequest();
    getMediaCommentsRequest->request("media/"+mediaId+"/comments/?"
                                     "ig_sig_key_version="+ SIG_KEY_VERSION +
                                     (max_id.length()>0 ? "&max_id="+max_id : "")
                                     ,NULL);
    QObject::connect(getMediaCommentsRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaCommentsDataReady(QVariant)));
}

void Instagramv2::getLikedMedia(QString max_id)
{
    InstagramRequest *getLikedMediaRequest = new InstagramRequest();
        getLikedMediaRequest->request("feed/liked/" +
                                      (max_id.length()>0 ? "?max_id="+max_id : "")
                                      ,NULL);

    QObject::connect(getLikedMediaRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(likedMediaDataReady(QVariant)));
}

void Instagramv2::getMediaLikers(QString mediaId)
{
    InstagramRequest *getMediaLikersRequest = new InstagramRequest();
    getMediaLikersRequest->request("media/"+mediaId+"/likers/",NULL);
    QObject::connect(getMediaLikersRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(mediaLikersDataReady(QVariant)));
}

