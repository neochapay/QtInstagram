#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>
#include <QUuid>

void Instagramv2::like(QString mediaId, QString module)
{
    InstagramRequestv2 *likeRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = likeRequest->generateSignature(data);
    likeRequest->request("media/"+mediaId+"/like/",signature.toUtf8());
    QObject::connect(likeRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likeDataReady(QVariant)));
}

void Instagramv2::unLike(QString mediaId, QString module)
{
    InstagramRequestv2 *unLikeRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = unLikeRequest->generateSignature(data);
    unLikeRequest->request("media/"+mediaId+"/unlike/",signature.toUtf8());
    QObject::connect(unLikeRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(unLikeDataReady(QVariant)));
}

void Instagramv2::getLikedFeed(QString max_id)
{
    InstagramRequestv2 *getLikedFeedRequest = new InstagramRequestv2();
    getLikedFeedRequest->request("feed/liked/"
                                 + (max_id.length()>0 ? "?max_id="+max_id : "")
                                 ,NULL);
    QObject::connect(getLikedFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likedFeedDataReady(QVariant)));
}

void Instagramv2::getInfoMedia(QString mediaId)
{
    InstagramRequestv2 *infoMediaRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("media_id", mediaId);

    QString signature = infoMediaRequest->generateSignature(data);
    infoMediaRequest->request("media/"+mediaId+"/info/"
                              ,signature.toUtf8());
    QObject::connect(infoMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaInfoReady(QVariant)));
}

void Instagramv2::deleteMedia(QString mediaId, QString mediaType)
{
    InstagramRequestv2 *deleteMediaRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("media_id",    mediaId);

    QString signature = deleteMediaRequest->generateSignature(data);
    deleteMediaRequest->request("media/"+mediaId+"/delete/?"
                                "media_type=" + mediaType
                                ,signature.toUtf8());
    QObject::connect(deleteMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaDeleted(QVariant)));
}

void Instagramv2::editMedia(QString mediaId, QString captionText, QString mediaType)
{
    InstagramRequestv2 *editMediaRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("caption_text", captionText);

    QString signature = editMediaRequest->generateSignature(data);
    editMediaRequest->request("media/"+mediaId+"/edit_media/"
                              ,signature.toUtf8());
    QObject::connect(editMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaEdited(QVariant)));
}

void Instagramv2::comment(QString mediaId, QString commentText, QString replyCommentId ,QString module)
{
    InstagramRequestv2 *postCommentRequest = new InstagramRequestv2();
    QUuid uuid;

    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        data.insert("comment_text", commentText);
        data.insert("containermoudle", module);
        data.insert("idempotence_token", uuid.createUuid().toString());
        data.insert("radio-type",   "wifi-none");
    if(replyCommentId != "" && replyCommentId.at(0) == '@')
        data.insert("replied_to_comment_id", replyCommentId);

    QString signature = postCommentRequest->generateSignature(data);
    postCommentRequest->request("media/"+mediaId+"/comment/",signature.toUtf8());
    QObject::connect(postCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentPosted(QVariant)));
}

void Instagramv2::deleteComment(QString mediaId, QString commentId)
{
    InstagramRequestv2 *deleteCommentRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);
        //data.insert("caption_text", captionText);

    QString signature = deleteCommentRequest->generateSignature(data);
    deleteCommentRequest->request("media/"+mediaId+"/comment/"+commentId+"/delete/",signature.toUtf8());
    QObject::connect(deleteCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentDeleted(QVariant)));
}

void Instagramv2::likeComment(QString commentId)
{
    InstagramRequestv2 *likeCommentRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);

    QString signature = likeCommentRequest->generateSignature(data);
    likeCommentRequest->request("media/"+commentId+"/comment_like/",signature.toUtf8());
    QObject::connect(likeCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentLiked(QVariant)));
}

void Instagramv2::unlikeComment(QString commentId)
{
    InstagramRequestv2 *unlikeCommentRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);

    QString signature = unlikeCommentRequest->generateSignature(data);
    unlikeCommentRequest->request("media/"+commentId+"/comment_unlike/",signature.toUtf8());
    QObject::connect(unlikeCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentUnliked(QVariant)));
}

void Instagramv2::getComments(QString mediaId, QString max_id)
{
    InstagramRequestv2 *getMediaCommentsRequest = new InstagramRequestv2();
    getMediaCommentsRequest->request("media/"+mediaId+"/comments/?"
                                     "ig_sig_key_version="+ SIG_KEY_VERSION +
                                     (max_id.length()>0 ? "&max_id="+max_id : "")
                                     ,NULL);
    QObject::connect(getMediaCommentsRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaCommentsDataReady(QVariant)));
}

void Instagramv2::getLikedMedia(QString max_id)
{
    InstagramRequestv2 *getLikedMediaRequest = new InstagramRequestv2();
        getLikedMediaRequest->request("feed/liked/" +
                                      (max_id.length()>0 ? "?max_id="+max_id : "")
                                      ,NULL);

    QObject::connect(getLikedMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likedMediaDataReady(QVariant)));
}

void Instagramv2::getMediaLikers(QString mediaId)
{
    InstagramRequestv2 *getMediaLikersRequest = new InstagramRequestv2();
    getMediaLikersRequest->request("media/"+mediaId+"/likers/",NULL);
    QObject::connect(getMediaLikersRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaLikersDataReady(QVariant)));
}
