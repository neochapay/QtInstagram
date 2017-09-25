#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>


//getUserTag
void Instagramv2::getUserTags(QString userId, QString max_id, QString minTimestamp)
{
    InstagramRequest *getUserTagsRequest = new InstagramRequest();
    getUserTagsRequest->request("usertags/"+userId+"/feed/?"
                                "rank_token="+this->m_rank_token+"&"
                                "ranked_content=true" +
                                (max_id.length()>0 ?  "&max_id="+max_id : "" ) +
                                (minTimestamp.length()>0? "&min_timestamp="+minTimestamp : "" )
                                ,NULL);
    QObject::connect(getUserTagsRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userTagsDataReady(QVariant)));
}


void Instagramv2::removeSelftag(QString mediaId)
{
    InstagramRequest *removeSelftagRequest = new InstagramRequest();
    QJsonObject data;
        data.insert("_uuid",        this->m_uuid);
        data.insert("_uid",         this->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+this->m_token);

    QString signature = removeSelftagRequest->generateSignature(data);
    removeSelftagRequest->request("usertags/"+mediaId+"/remove/",signature.toUtf8());
    QObject::connect(removeSelftagRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(removeSelftagDone(QVariant)));
}

