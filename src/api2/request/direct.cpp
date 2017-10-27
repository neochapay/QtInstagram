#include "../instagramv2_p.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getInbox(QString cursorId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getInboxRequest =
        d->request("direct_v2/inbox/?"
                   "persistentBadging=true&"
                   "use_unified_inbox=true"+
                   (cursorId.length()>0 ? "&cursor="+cursorId : "")
                   , NULL);
    QObject::connect(getInboxRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(inboxDataReady(QVariant)));
}

void Instagramv2::getPendingInbox()
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getPendingInboxRequest =
        d->request("direct_v2/pending_inbox/"
                   "persistentBadging=true&"
                   "use_unified_inbox=true"
                   , NULL);
    QObject::connect(getPendingInboxRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(pendingInboxDataReady(QVariant)));
}

void Instagramv2::getDirectThread(QString threadId, QString cursorId)
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getDirectThreadRequest =
        d->request("direct_v2/threads/"+threadId+"/?"
                   "use_unified_inbox=true"+
                   (cursorId.length()>0 ? "&cursor="+cursorId : "")
                   , NULL);
    QObject::connect(getDirectThreadRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(directThreadDataReady(QVariant)));
}

void Instagramv2::getRecentRecipients()
{
    Q_D(Instagramv2);

    InstagramRequestv2 *getRecentRecipientsRequest =

        d->request("direct_share/recent_recipients/", NULL);
    QObject::connect(getRecentRecipientsRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(recentRecipientsDataReady(QVariant)));

}
