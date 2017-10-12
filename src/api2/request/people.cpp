#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getInfoByName(QString username)
{
    InstagramRequestv2 *getInfoByNameRequest = new InstagramRequestv2();
    getInfoByNameRequest->request("users/"+username+"/usernameinfo/",NULL);
    QObject::connect(getInfoByNameRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(infoByNameDataReady(QVariant)));
}

//old getUsernameInfo
void Instagramv2::getInfoById(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getInfoByIdRequest = new InstagramRequestv2();
    getInfoByIdRequest->request("users/"+userId+"/info/"
                                "?device_id="+d->m_device_id
                                ,NULL);
    QObject::connect(getInfoByIdRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(infoByIdDataReady(QVariant)));
}

//old getRecentActivity
void Instagramv2::getRecentActivityInbox()
{
    InstagramRequestv2 *getRecentActivityInboxRequest = new InstagramRequestv2();
    getRecentActivityInboxRequest->request("news/inbox/?"
                                      "activity_module=all&"
                                      "show_su=true"
                                      ,NULL);
    QObject::connect(getRecentActivityInboxRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(recentActivityInboxDataReady(QVariant)));
}

void Instagramv2::getFollowingRecentActivity()
{
    InstagramRequestv2 *getFollowingRecentActivityRequest = new InstagramRequestv2();
    getFollowingRecentActivityRequest->request("news/?",NULL);
    QObject::connect(getFollowingRecentActivityRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(followingRecentActivityDataReady(QVariant)));
}

void Instagramv2::getFriendship(QString userId)
{
    InstagramRequestv2 *getFriendshipRequest = new InstagramRequestv2();

    getFriendshipRequest->request("friendships/show/"+userId+"/",NULL);
    QObject::connect(getFriendshipRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(friendshipDataReady(QVariant)));
}

void Instagramv2::getFollowing(QString userId, QString max_id, QString searchQuery)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getFollowingRequest = new InstagramRequestv2();
    getFollowingRequest->request("friendships/"+userId+"/following/?"
                                 "rank_token="+d->m_rank_token +
                                 (max_id.length()>0 ? "&max_id="+max_id : "") +
                                 (searchQuery.length()>0 ? "&query="+searchQuery : "")
                                  ,NULL);
    QObject::connect(getFollowingRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(followingDataReady(QVariant)));
}

void Instagramv2::getFollowers(QString userId, QString max_id, QString searchQuery)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getFollowersRequest = new InstagramRequestv2();
    getFollowersRequest->request("friendships/"+userId+"/followers/?"
                                 "rank_token="+d->m_rank_token +
                                 (max_id.length()>0 ? "&max_id="+max_id : "") +
                                 (searchQuery.length()>0 ? "&query="+searchQuery : "")
                                 ,NULL);
    QObject::connect(getFollowersRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(followersDataReady(QVariant)));
}

//userFeed
void Instagramv2::searchUser(QString query)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getSearchUserRequest = new InstagramRequestv2();
    getSearchUserRequest->request("users/search/?"
                                "query="+query+
                                "&is_typeahead=true&"
                                "rank_token="+d->m_rank_token+
                                "&ig_sig_key_version="+Constants::sigKeyVersion()
                                ,NULL);
    QObject::connect(getSearchUserRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(searchUserDataReady(QVariant)));
}

void Instagramv2::follow(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *followRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    data.insert("user_id",      userId);
    data.insert("radio_type",   "wifi-none");
    QString signature = followRequest->generateSignature(data);

    followRequest->request("friendships/create/"+userId+"/"
                           ,signature.toUtf8());
    QObject::connect(followRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(followDataReady(QVariant)));
}

void Instagramv2::unFollow(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *unFollowRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    data.insert("user_id",      userId);
    data.insert("radio_type",   "wifi-none");
    QString signature = unFollowRequest->generateSignature(data);

    unFollowRequest->request("friendships/destroy/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(unFollowRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(unfollowDataReady(QVariant)));
}

void Instagramv2::favorite(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *favoriteRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    QString signature = favoriteRequest->generateSignature(data);

    favoriteRequest->request("friendships/favorite/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(favoriteRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(favoriteDataReady(QVariant)));
}

void Instagramv2::unFavorite(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *unFavoriteRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    QString signature = unFavoriteRequest->generateSignature(data);

    unFavoriteRequest->request("friendships/unfavorite/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(unFavoriteRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(unFavoriteDataReady(QVariant)));
}

void Instagramv2::block(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *blockRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    data.insert("user_id",      userId);
    QString signature = blockRequest->generateSignature(data);
    blockRequest->request("friendships/block/" + userId + "/?"
                          ,signature.toUtf8());
    QObject::connect(blockRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(blockDataReady(QVariant)));
}

void Instagramv2::unBlock(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *unBlockRequest = new InstagramRequestv2();
    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_uid",         d->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    data.insert("user_id",      userId);

    QString signature = unBlockRequest->generateSignature(data);
    unBlockRequest->request("friendships/unblock/" + userId + "/"
                            ,signature.toUtf8());
    QObject::connect(unBlockRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(unBlockDataReady(QVariant)));
}

void Instagramv2::getSugestedUser(QString userId) {
    InstagramRequestv2 *getSuggestedRequest = new InstagramRequestv2();


    getSuggestedRequest->request("discover/chaining/?"
                                 "target_id="+userId
                             ,NULL);
    QObject::connect(getSuggestedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(suggestedUserDataReady(QVariant)));
}
