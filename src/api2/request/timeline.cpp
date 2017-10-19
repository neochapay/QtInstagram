#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getTimelineFeed(QString max_id)
{
    Q_D(Instagramv2);

    QJsonObject data;
    data.insert("_uuid",        d->m_uuid);
    data.insert("_csrftoken",   "Set-Cookie: csrftoken="+d->m_token);
    data.insert("battery_level","100");
    data.insert("is_prefetch", "0");
    if(max_id.length()>0) data.insert("max_id",max_id);
    QString signature = InstagramRequestv2::generateSignature(data);

    QString target ="feed/timeline/?"
                    "rank_token="+d->m_rank_token+
                    "&ranked_content=false&_uuid="+d->m_uuid+
                    (max_id.length()>0 ?  "&max_id="+max_id  : "" );

    // "feed/timeline/"
    InstagramRequestv2 *getTimeLineFeedRequest =
        d->request(target,signature.toUtf8());

    QObject::connect(getTimeLineFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(timelineFeedDataReady(QVariant)));
}

//getUserTimeLine
void Instagramv2::getUserFeed(QString userID, QString max_id, QString minTimestamp)
{
    Q_D(Instagramv2);

    QString target = "feed/user/"+userID+"/?"
                     "rank_token="+d->m_rank_token +
                     (max_id.length()>0 ?  "&max_id="+max_id  : "" ) +
                     (minTimestamp.length()>0? "&min_timestamp="+minTimestamp : "" ) +
                     "&ranked_content=true";


    InstagramRequestv2 *getUserFeedRequest = d->request(target,NULL);
    QObject::connect(getUserFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(userFeedDataReady(QVariant)));
}
