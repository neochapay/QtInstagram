#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>

void Instagramv2::getExploreFeed(QString max_id, QString isPrefetch)
{
    InstagramRequest *getExploreRequest = new InstagramRequest();
    getExploreRequest->request("discover/explore/?"
                               "is_prefetch="+isPrefetch+"&"
                               "is_from_promote=false&"
                               "session_id=" + this->m_token +
                               "&module=explore_popular" +
                               (max_id.length()>0 ? "&max_id="+max_id : "" )
                               ,NULL);
    QObject::connect(getExploreRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(exploreFeedDataReady(QVariant)));
}
