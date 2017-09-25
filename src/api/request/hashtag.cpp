#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>


void Instagramv2::getTagFeed(QString tag, QString max_id)
{
    InstagramRequest *getTagFeedRequest = new InstagramRequest();
    getTagFeedRequest->request("feed/tag/"+tag.toUtf8()+
                               "/?rank_token="+this->m_rank_token +
                               "&ranked_content=true"+
                               (max_id.length()>0 ? "&max_id="+max_id : "" )
                               ,NULL);
    QObject::connect(getTagFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(tagFeedDataReady(QVariant)));
}
