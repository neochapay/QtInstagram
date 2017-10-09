#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

//getUserTag
void Instagramv2::getUserTags(QString userId, QString max_id, QString minTimestamp)
{
    InstagramRequestv2 *getUserTagsRequest = new InstagramRequestv2();
    getUserTagsRequest->request("usertags/"+userId+"/feed/?"
                                "rank_token="+m_rank_token+"&"
                                "ranked_content=true" +
                                (max_id.length()>0 ?  "&max_id="+max_id : "" ) +
                                (minTimestamp.length()>0? "&min_timestamp="+minTimestamp : "" )
                                ,NULL);
    QObject::connect(getUserTagsRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(userTagsDataReady(QVariant)));
}

void Instagramv2::removeSelftag(QString mediaId)
{
    InstagramRequestv2 *removeSelftagRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        m_uuid);
        data.insert("_uid",         m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+m_token);

    QString signature = removeSelftagRequest->generateSignature(data);
    removeSelftagRequest->request("usertags/"+mediaId+"/remove/",signature.toUtf8());
    QObject::connect(removeSelftagRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(removeSelftagDone(QVariant)));
}

