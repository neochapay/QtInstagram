#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getTagFeed(QString tag, QString max_id)
{
    InstagramRequestv2 *getTagFeedRequest = new InstagramRequestv2();
    getTagFeedRequest->request("feed/tag/"+tag.toUtf8()+
                               "/?rank_token="+m_rank_token +
                               "&ranked_content=true"+
                               (max_id.length()>0 ? "&max_id="+max_id : "" )
                               ,NULL);
    QObject::connect(getTagFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(tagFeedDataReady(QVariant)));
}
