#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>

void Instagramv2::getReelsTrayFeed()
{
    InstagramRequest *getReelsTrayFeedRequest = new InstagramRequest();
    getReelsTrayFeedRequest->request("feed/reels_tray/"
                                     ,NULL);
    QObject::connect(getReelsTrayFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(reelsTrayFeedDataReady(QVariant)));
}

void Instagramv2::getUserReelsMediaFeed(QString userId)
{
    InstagramRequest *getUserReelsMediaFeedRequest = new InstagramRequest();
    getUserReelsMediaFeedRequest->request("feed/user/"+userId+"/reel_media/"
                                     ,NULL);
    QObject::connect(getUserReelsMediaFeedRequest,SIGNAL(replySrtingReady(QVariant)),this,SIGNAL(userReelsMediaFeedDataReady(QVariant)));
}
