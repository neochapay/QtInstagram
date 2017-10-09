#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getReelsTrayFeed()
{
    InstagramRequestv2 *getReelsTrayFeedRequest = new InstagramRequestv2();
    getReelsTrayFeedRequest->request("feed/reels_tray/"
                                     ,NULL);
    QObject::connect(getReelsTrayFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(reelsTrayFeedDataReady(QVariant)));
}

void Instagramv2::getUserReelsMediaFeed(QString userId)
{
    InstagramRequestv2 *getUserReelsMediaFeedRequest = new InstagramRequestv2();
    getUserReelsMediaFeedRequest->request("feed/user/"+userId+"/reel_media/"
                                     ,NULL);
    QObject::connect(getUserReelsMediaFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(userReelsMediaFeedDataReady(QVariant)));
}
