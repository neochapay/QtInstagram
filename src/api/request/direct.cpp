#include "../instagramv2.h"
#include "../instagramrequest.h"
#include <QJsonObject>

void Instagramv2::getInbox(QString cursorId)
{
    InstagramRequest *getInboxRequest = new InstagramRequest();
    getInboxRequest->request("direct_v2/inbox/?"
                             "persistentBadging=true&"
                             "use_unified_inbox=true"+
                             (cursorId.length()>0 ? "&cursor="+cursorId : "")
                             , NULL);
    QObject::connect(getInboxRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(inboxDataReady(QVariant)));
}

void Instagramv2::getPendingInbox()
{
    InstagramRequest *getPendingInboxRequest = new InstagramRequest();
    getPendingInboxRequest->request("direct_v2/pending_inbox/"
                                    "persistentBadging=true&"
                                    "use_unified_inbox=true"
                                    , NULL);
    QObject::connect(getPendingInboxRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(pendingInboxDataReady(QVariant)));
}

void Instagramv2::getDirectThread(QString threadId, QString cursorId)
{
    InstagramRequest *getDirectThreadRequest = new InstagramRequest();
    getDirectThreadRequest->request("direct_v2/threads/"+threadId+"/"
                                    "use_unified_inbox=true"+
                                    (cursorId.length()>0 ? "&cursor="+cursorId : "")
                                    , NULL);
    QObject::connect(getDirectThreadRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(directThreadDataReady(QVariant)));
}


void Instagramv2::getRecentRecipients()
{
    InstagramRequest *getRecentRecipientsRequest = new InstagramRequest();
    //QJsonObject data;
        //data.insert("_uuid",        this->m_uuid);
        //data.insert("_csrftoken",   QString("missing"));
        //data.insert("show_threads", QString("true"));

    //QString signature = getRecentRecipientsRequest->generateSignature(data);
    getRecentRecipientsRequest->request("direct_share/recent_recipients/", NULL);
    QObject::connect(getRecentRecipientsRequest,SIGNAL(replySrtingReady(QVariant)), this, SIGNAL(recentRecipientsDataReady(QVariant)));

}
