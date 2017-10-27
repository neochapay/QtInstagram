#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getReelsTrayFeed()
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getReelsTrayFeedRequest =
        d->request("feed/reels_tray/"
                   ,NULL);
    QObject::connect(getReelsTrayFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(reelsTrayFeedDataReady(QVariant)));
}

void Instagramv2::getUserReelsMediaFeed(QString userId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getUserReelsMediaFeedRequest =
        d->request("feed/user/"+userId+"/reel_media/"
                   ,NULL);
    QObject::connect(getUserReelsMediaFeedRequest,SIGNAL(replyStringReady(QVariant)),this,SIGNAL(userReelsMediaFeedDataReady(QVariant)));
}
