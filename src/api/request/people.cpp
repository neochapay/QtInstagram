#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>

void Instagramv2::getInfoByName(QString username)
{
    InstagramRequest *getInfoByNameRequest = new InstagramRequest();
    getInfoByNameRequest->request("users/"+username+"/usernameinfo/",NULL);
    QObject::connect(getInfoByNameRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(infoByNameDataReady(QVariant)));
}


//old getUsernameInfo
void Instagramv2::getInfoById(QString userId)
{
    InstagramRequest *getInfoByIdRequest = new InstagramRequest();
    getInfoByIdRequest->request("users/"+userId+"/info/"
                                "?device_id="+this->m_device_id
                                ,NULL);
    QObject::connect(getInfoByIdRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(infoByIdDataReady(QVariant)));
}

//old getRecentActivity
void Instagramv2::getRecentActivityInbox()
{
    InstagramRequest *getRecentActivityInboxRequest = new InstagramRequest();
    getRecentActivityInboxRequest->request("news/inbox/?"
                                      "activity_module=all&"
                                      "show_su=true"
                                      ,NULL);
    QObject::connect(getRecentActivityInboxRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(recentActivityInboxDataReady(QVariant)));
}

void Instagramv2::getFollowingRecentActivity()
{
    InstagramRequest *getFollowingRecentActivityRequest = new InstagramRequest();
    getFollowingRecentActivityRequest->request("news/?",NULL);
    QObject::connect(getFollowingRecentActivityRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followingRecentActivityDataReady(QVariant)));
}

void Instagramv2::getFriendship(QString userId)
{
    InstagramRequest *getFriendshipRequest = new InstagramRequest();

    getFriendshipRequest->request("friendships/show/"+userId+"/",NULL);
    QObject::connect(getFriendshipRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(friendshipDataReady(QVariant)));
}

void Instagramv2::getFollowing(QString userId, QString max_id, QString searchQuery)
{
    InstagramRequest *getFollowingRequest = new InstagramRequest();
    //QJsonObject data;
       //data.insert("_uuid",        this->m_uuid);
        //data.insert("_uid",         this->m_username_id);
        //data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    //if(maxId != ""){
        //data.insert("max_id",     maxId);
    //}

    //QString signature = getUserFollowingsRequest->generateSignature(data);

    getFollowingRequest->request("friendships/"+userId+"/following/?"
                                 "rank_token="+this->m_rank_token +
                                 (max_id.length()>0 ? "&max_id="+max_id : "") +
                                 (searchQuery.length()>0 ? "&query="+searchQuery : "")
                                  ,NULL);
    QObject::connect(getFollowingRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followingDataReady(QVariant)));
}

void Instagramv2::getFollowers(QString userId, QString max_id, QString searchQuery)
{
    //qDebug() << userId;
    InstagramRequest *getFollowersRequest = new InstagramRequest();
    //QJsonObject data;
        //data.insert("_uuid",        this->m_uuid);
        //data.insert("_uid",         this->m_username_id);
        //data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    //if(maxId != ""){
        //data.insert("max_id",     maxId);
    //}

    //QString signature = getUserFollowersRequest->generateSignature(data);
    getFollowersRequest->request("friendships/"+userId+"/followers/"
                                 "rank_token="+this->m_rank_token +
                                 (max_id.length()>0 ? "&max_id="+max_id : "") +
                                 (searchQuery.length()>0 ? "&query="+searchQuery : "")
                                 ,NULL);
    QObject::connect(getFollowersRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followersDataReady(QVariant)));
}

//userFeed
void Instagramv2::searchUser(QString query)
{
    InstagramRequest *getSearchUserRequest = new InstagramRequest();
    getSearchUserRequest->request("users/search/?"
                                "query="+query+
                                "&is_typeahead=true&"
                                "rank_token="+this->m_rank_token+
                                "&ig_sig_key_version="+SIG_KEY_VERSION
                                ,NULL);
    QObject::connect(getSearchUserRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(searchUserDataReady(QVariant)));
}

void Instagramv2::follow(QString userId)
{
    InstagramRequest *followRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    data.insert("user_id",      userId);
    data.insert("radio_type",   "wifi-none");
    QString signature = followRequest->generateSignature(data);

    followRequest->request("friendships/create/"+userId+"/"
                           ,signature.toUtf8());
    QObject::connect(followRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(followDataReady(QVariant)));
}

void Instagramv2::unFollow(QString userId)
{
    InstagramRequest *unFollowRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    data.insert("user_id",      userId);
    data.insert("radio_type",   "wifi-none");
    QString signature = unFollowRequest->generateSignature(data);

    unFollowRequest->request("friendships/destroy/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(unFollowRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unFollowDataReady(QVariant)));
}

void Instagramv2::favorite(QString userId)
{
    InstagramRequest *favoriteRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    QString signature = favoriteRequest->generateSignature(data);

    favoriteRequest->request("friendships/favorite/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(favoriteRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(favoriteDataReady(QVariant)));
}

void Instagramv2::unFavorite(QString userId)
{
    InstagramRequest *unFavoriteRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    QString signature = unFavoriteRequest->generateSignature(data);

    unFavoriteRequest->request("friendships/unfavorite/"+userId+"/"
                             ,signature.toUtf8());
    QObject::connect(unFavoriteRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unFavoriteDataReady(QVariant)));
}

void Instagramv2::block(QString userId)
{
    InstagramRequest *blockRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    data.insert("user_id",      userId);
    QString signature = blockRequest->generateSignature(data);
    blockRequest->request("friendships/block/" + userId + "/?"
                          ,signature.toUtf8());
    QObject::connect(blockRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(blockDataReady(QVariant)));
}

void Instagramv2::unBlock(QString userId)
{
    InstagramRequest *unBlockRequest = new InstagramRequest();
    QJsonObject data;
    data.insert("_uuid",        this->m_uuid);
    data.insert("_uid",         this->m_username_id);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);
    data.insert("user_id",      userId);

    QString signature = unBlockRequest->generateSignature(data);
    unBlockRequest->request("friendships/unblock/" + userId + "/"
                            ,signature.toUtf8());
    QObject::connect(unBlockRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(unBlockDataReady(QVariant)));
}

void Instagramv2::getSugestedUser(QString userId) {
    InstagramRequest *getSuggestedRequest = new InstagramRequest();


    getSuggestedRequest->request("discover/chaining/?"
                                 "target_id="+userId
                             ,NULL);
    QObject::connect(getSuggestedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(suggestedUserDataReady(QVariant)));



}
