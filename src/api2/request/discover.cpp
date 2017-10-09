#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getExploreFeed(QString max_id, QString isPrefetch)
{
    InstagramRequestv2 *getExploreRequest = new InstagramRequestv2();
    getExploreRequest->request("discover/explore/?"
                               "is_prefetch="+isPrefetch+"&"
                               "is_from_promote=false&"
                               "session_id=" + m_token +
                               "&module=explore_popular" +
                               (max_id.length()>0 ? "&max_id="+max_id : "" )
                               ,NULL);
    QObject::connect(getExploreRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(exploreFeedDataReady(QVariant)));
}
