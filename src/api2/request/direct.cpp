#include "../instagramv2.h"
#include "../instagramrequestv2.h"
#include <QJsonObject>

void Instagramv2::getInbox(QString cursorId)
{
    InstagramRequestv2 *getInboxRequest = new InstagramRequestv2();
    getInboxRequest->request("direct_v2/inbox/?"
                             "persistentBadging=true&"
                             "use_unified_inbox=true"+
                             (cursorId.length()>0 ? "&cursor="+cursorId : "")
                             , NULL);
    QObject::connect(getInboxRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(inboxDataReady(QVariant)));
}

void Instagramv2::getPendingInbox()
{
    InstagramRequestv2 *getPendingInboxRequest = new InstagramRequestv2();
    getPendingInboxRequest->request("direct_v2/pending_inbox/"
                                    "persistentBadging=true&"
                                    "use_unified_inbox=true"
                                    , NULL);
    QObject::connect(getPendingInboxRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(pendingInboxDataReady(QVariant)));
}

void Instagramv2::getDirectThread(QString threadId, QString cursorId)
{
    InstagramRequestv2 *getDirectThreadRequest = new InstagramRequestv2();
    getDirectThreadRequest->request("direct_v2/threads/"+threadId+"/?"
                                    "use_unified_inbox=true"+
                                    (cursorId.length()>0 ? "&cursor="+cursorId : "")
                                    , NULL);
    QObject::connect(getDirectThreadRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(directThreadDataReady(QVariant)));
}

void Instagramv2::getRecentRecipients()
{
    InstagramRequestv2 *getRecentRecipientsRequest = new InstagramRequestv2();

    getRecentRecipientsRequest->request("direct_share/recent_recipients/", NULL);
    QObject::connect(getRecentRecipientsRequest,SIGNAL(replyStringReady(QVariant)), this, SIGNAL(recentRecipientsDataReady(QVariant)));

}
