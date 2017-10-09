#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

//getUserTag
void Instagramv2::getUserTags(QString userId, QString max_id, QString minTimestamp)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getUserTagsRequest = new InstagramRequestv2();
    getUserTagsRequest->request("usertags/"+userId+"/feed/?"
                                "rank_token="+d->m_rank_token+"&"
                                "ranked_content=true" +
                                (max_id.length()>0 ?  "&max_id="+max_id : "" ) +
                                (minTimestamp.length()>0? "&min_timestamp="+minTimestamp : "" )
                                ,NULL);
    QObject::connect(getUserTagsRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(userTagsDataReady(QVariant)));
}

void Instagramv2::removeSelftag(QString mediaId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *removeSelftagRequest = new InstagramRequestv2();
    QJsonObject data;
        data.insert("_uuid",        d->m_uuid);
        data.insert("_uid",         d->m_username_id);
        data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);

    QString signature = removeSelftagRequest->generateSignature(data);
    removeSelftagRequest->request("usertags/"+mediaId+"/remove/",signature.toUtf8());
    QObject::connect(removeSelftagRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(removeSelftagDone(QVariant)));
}

