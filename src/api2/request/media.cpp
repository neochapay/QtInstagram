#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>
#include <QUuid>

void Instagramv2::like(QString mediaId, QString module)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *likeRequest =
        d->request("media/"+mediaId+"/like/",signature.toUtf8());
    QObject::connect(likeRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likeDataReady(QVariant)));
}

void Instagramv2::unLike(QString mediaId, QString module)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("media_id",     mediaId);
        data.insert("radio-type",   "wifi-none");
        data.insert("module_name",  module);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *unLikeRequest =
        d->request("media/"+mediaId+"/unlike/",signature.toUtf8());
    QObject::connect(unLikeRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(unLikeDataReady(QVariant)));
}

void Instagramv2::getLikedFeed(QString max_id)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getLikedFeedRequest =
        d->request("feed/liked/"
                   + (max_id.length()>0 ? "?max_id="+max_id : "")
                   ,NULL);
    QObject::connect(getLikedFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likedFeedDataReady(QVariant)));
}

void Instagramv2::getInfoMedia(QString mediaId)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("media_id", mediaId);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *infoMediaRequest =
        d->request("media/"+mediaId+"/info/"
                   ,signature.toUtf8());
    QObject::connect(infoMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaInfoReady(QVariant)));
}

void Instagramv2::deleteMedia(QString mediaId, QString mediaType)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("media_id",    mediaId);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *deleteMediaRequest =
        d->request("media/"+mediaId+"/delete/?"
                   "media_type=" + mediaType
                   ,signature.toUtf8());
    QObject::connect(deleteMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaDeleted(QVariant)));
}

void Instagramv2::editMedia(QString mediaId, QString captionText, QString mediaType)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("caption_text", captionText);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *editMediaRequest =
        d->request("media/"+mediaId+"/edit_media/"
                   ,signature.toUtf8());
    QObject::connect(editMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaEdited(QVariant)));
}

void Instagramv2::comment(QString mediaId, QString commentText, QString replyCommentId ,QString module)
{
    Q_D(Instagramv2);

    QUuid uuid;

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        data.insert("comment_text", commentText);
        data.insert("containermoudle", module);
        data.insert("idempotence_token", uuid.createUuid().toString());
        data.insert("radio-type",   "wifi-none");
    if(replyCommentId != "" && replyCommentId.at(0) == '@')
        data.insert("replied_to_comment_id", replyCommentId);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *postCommentRequest =
        d->request("media/"+mediaId+"/comment/",signature.toUtf8());
    QObject::connect(postCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentPosted(QVariant)));
}

void Instagramv2::deleteComment(QString mediaId, QString commentId)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
        //data.insert("caption_text", captionText);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *deleteCommentRequest =
        d->request("media/"+mediaId+"/comment/"+commentId+"/delete/",signature.toUtf8());
    QObject::connect(deleteCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentDeleted(QVariant)));
}

void Instagramv2::likeComment(QString commentId)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *likeCommentRequest =
        d->request("media/"+commentId+"/comment_like/",signature.toUtf8());
    QObject::connect(likeCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentLiked(QVariant)));
}

void Instagramv2::unlikeComment(QString commentId)
{
    Q_D(Instagramv2);

    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);

    QString signature = InstagramRequestv2::generateSignature(data);
    InstagramRequestv2 *unlikeCommentRequest =
        d->request("media/"+commentId+"/comment_unlike/",signature.toUtf8());
    QObject::connect(unlikeCommentRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(commentUnliked(QVariant)));
}

void Instagramv2::getComments(QString mediaId, QString max_id)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getMediaCommentsRequest =
        d->request("media/"+mediaId+"/comments/?"
                   "ig_sig_key_version="+ Constants::sigKeyVersion() +
                   (max_id.length()>0 ? "&max_id="+max_id : "")
                   ,NULL);
    QObject::connect(getMediaCommentsRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaCommentsDataReady(QVariant)));
}

void Instagramv2::getLikedMedia(QString max_id)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getLikedMediaRequest =
        d->request("feed/liked/" +
                   (max_id.length()>0 ? "?max_id="+max_id : "")
                   ,NULL);

    QObject::connect(getLikedMediaRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(likedMediaDataReady(QVariant)));
}

void Instagramv2::getMediaLikers(QString mediaId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getMediaLikersRequest =
        d->request("media/"+mediaId+"/likers/",NULL);
    QObject::connect(getMediaLikersRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(mediaLikersDataReady(QVariant)));
}
