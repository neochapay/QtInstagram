#include "QtInstagram"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QScopedPointer>
#include <QSignalSpy>
#include <QTest>
#include <QUuid>
#include <QUrlQuery>
#include "fake_network.h"

typedef std::function<void(Instagram &)> TestedMethod;
Q_DECLARE_METATYPE(TestedMethod)

class QtInstagramTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testLogin_data();
    void testLogin();

    void testBodilessRequests_data();
    void testBodilessRequests();

    // media endpoint
    void testLike();
    void testUnlike();
    void testGetInfoMedia();
    void testDeleteMedia();
    void testEditMedia();
    void testComment_data();
    void testComment();
    void testDeleteComment();
    void testLikeComment();
    void testUnlikeComment();

    // people endpoint
    void testGetInfoById();

private:
    QString extractUuid(const QUrl &url) const;
    QJsonObject extractSignedData(const QByteArray &body) const;
    void doLogin(Instagram *instagram);
    void sendResponseAndCheckSignal(Instagram *instagram, FakeReply *reply,
                                    const char *signalName);
    bool checkUuid(const QString &uuid);

private:
    QString m_usernameId = "789000123";
    QString m_uuid;
    QString m_rankToken;
    QJsonObject m_defaultHeaders;
};

QString QtInstagramTest::extractUuid(const QUrl &url) const
{
    return QUrlQuery(url).queryItemValue("guid");
}

QJsonObject QtInstagramTest::extractSignedData(const QByteArray &body) const
{
    QRegularExpression re(R"(ig_sig_key_version=4&signed_body=[^.]*\.(.*))");
    QRegularExpressionMatch match = re.match(QString::fromUtf8(body));

    if (!match.hasMatch()) return QJsonObject();

    QString data = match.captured(1);
    return QJsonDocument::fromJson(data.toUtf8()).object();
}

void QtInstagramTest::sendResponseAndCheckSignal(Instagram *instagram,
                                                 FakeReply *reply,
                                                 const char *signalName)
{
    /* send the response */
    QSignalSpy expectedSignal(instagram, signalName);
    QJsonObject data {
        { "something_here", "yes" },
        { "just arbitrary data", 12 },
    };
    reply->setData(QJsonDocument(data).toJson());
    QTRY_COMPARE(expectedSignal.count(), 1);
    QByteArray response = expectedSignal.at(0).at(0).toString().toUtf8();
    QCOMPARE(QJsonDocument::fromJson(response), QJsonDocument(data));
}

void QtInstagramTest::doLogin(Instagram *instagram)
{
    FakeNam *dummyNam = qobject_cast<FakeNam*>(instagram->networkAccessManager());
    QSignalSpy requestCreated(dummyNam, &FakeNam::requestCreated);

    instagram->setUsername("just_me");
    instagram->setPassword("my secret");
    QSignalSpy profileConnected(instagram, &Instagram::profileConnected);
    instagram->login();

    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();
    m_uuid = extractUuid(reply->url());
    m_rankToken = m_usernameId + "_" + m_uuid;

    // send a successful reply
    reply->setCookie("Set-Cookie: csrftoken=abc;"
                     " expires=Thu, 14-Dec-2027 08:50:19 GMT;"
                     " Max-Age=31449600; Path=/; secure");
    reply->setData("{\"status\": \"ok\"}");

    QTRY_COMPARE(requestCreated.count(), 2);
    reply = requestCreated.at(1).at(0).value<FakeReply*>();


    // Sent the login reply
    reply->setData("{\"logged_in_user\": {\"pk\": 789000123}}");
    QTRY_COMPARE(profileConnected.count(), 1);
}

bool QtInstagramTest::checkUuid(const QString &uuid)
{
    return !QUuid(uuid).isNull();
}

void QtInstagramTest::initTestCase()
{
    m_defaultHeaders = QJsonObject {
        { "Accept", "*/*" },
        { "Accept-Language", "en-US" },
        { "Connection", "close" },
        { "Content-type", "application/x-www-form-urlencoded; charset=UTF-8" },
        { "Cookie2", "$Version=1" },
        { "User-Agent", "Instagram 10.33.0 Android (18/4.3; 320dpi; 720x1280; Xiaomi; HM 1SW; armani; qcom; en_US)"},
    };
}

void QtInstagramTest::testLogin_data()
{
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("loginResponse");
    QTest::addColumn<QString>("expectedUsernameId");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("empty username") <<
        "" <<
        "validpassword" <<
        "{}" <<
        "" <<
        "Username and/or password is clean";

    QTest::newRow("empty password") <<
        "bob" <<
        "" <<
        "{}" <<
        "" <<
        "Username and/or password is clean";

    QTest::newRow("valid") <<
        "john_the_tester" <<
        "rootme" <<
        "{"
        "  \"logged_in_user\": {"
        "    \"pk\": 789000123"
        "  }"
        "}" <<
        "789000123" <<
        "";

    QTest::newRow("login failure") <<
        "smart_guy" <<
        "a rather long password" <<
        "{"
        "  \"status\": \"fail\","
        "  \"message\": \"you certainly did something wrong\""
        "}" <<
        "" <<
        "you certainly did something wrong";
}

void QtInstagramTest::testLogin()
{
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(QString, loginResponse);
    QFETCH(QString, expectedUsernameId);
    QFETCH(QString, expectedError);

    Instagram instagram;
    QSignalSpy profileConnected(&instagram, &Instagram::profileConnected);
    QSignalSpy profileConnectedFail(&instagram, &Instagram::profileConnectedFail);
    QSignalSpy error(&instagram, &Instagram::error);

    FakeNam dummyNam;
    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);
    instagram.setNetworkAccessManager(&dummyNam);

    instagram.setUsername(username);
    instagram.setPassword(password);
    instagram.login();

    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QString uuid = extractUuid(reply->url());
    QVERIFY(!uuid.isEmpty());

    QCOMPARE(reply->url(),
             QUrl("https://i.instagram.com/api/v1/si/fetch_headers/?challenge_type=signup&guid=" + uuid));

    QCOMPARE(reply->headers(), m_defaultHeaders);
    QVERIFY(reply->body().isEmpty());

    if (error.count() > 0 && !expectedError.isEmpty()) {
        QCOMPARE(error.at(0).at(0).toString(), expectedError);
        return;
    }

    // Send a successful reply
    reply->setCookie("Set-Cookie: csrftoken=abc;"
                     " expires=Thu, 14-Dec-2027 08:50:19 GMT;"
                     " Max-Age=31449600; Path=/; secure");

    reply->setData("{\"status\": \"ok\"}");
    QTRY_COMPARE(requestCreated.count(), 2);
    reply = requestCreated.at(1).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/accounts/login/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject loginData = extractSignedData(reply->body());
    QCOMPARE(loginData["phone_id"].toString().length(), 38);
    QCOMPARE(loginData["username"].toString(), username);
    QCOMPARE(loginData["password"].toString(), password);
    QCOMPARE(loginData["login_attempt_count"].toString(), QString("0"));
    QCOMPARE(loginData["guid"].toString(), uuid);
    QCOMPARE(loginData["_csrftoken"].toString(), QString("Set-Cookie: csrftoken=abc"));

    // Sent the login reply
    reply->setData(loginResponse.toUtf8());
    if (expectedUsernameId.isEmpty()) {
        QTRY_COMPARE(profileConnectedFail.count(), 1);
        QCOMPARE(profileConnected.count(), 0);
        QCOMPARE(error.count(), 1);
        QCOMPARE(error.at(0).at(0).toString(), expectedError);
    } else {
        QTRY_COMPARE(profileConnected.count(), 1);
        QCOMPARE(profileConnectedFail.count(), 0);
        QCOMPARE(error.count(), 0);
        QCOMPARE(instagram.getUsernameId(), expectedUsernameId);

        /* make sure that syncFeatures is being called */
        QTRY_COMPARE(requestCreated.count(), 3);
        reply = requestCreated.at(2).at(0).value<FakeReply*>();

        QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/qe/sync/"));
        QCOMPARE(reply->headers(), m_defaultHeaders);

        QJsonObject syncData = extractSignedData(reply->body());
        QCOMPARE(syncData["password"].toString(), password);
        QCOMPARE(syncData["_uuid"].toString(), uuid);
        QCOMPARE(syncData["_csrftoken"].toString(), QString("Set-Cookie: csrftoken=abc"));
        QCOMPARE(syncData["_uid"].toString(), expectedUsernameId);
        QCOMPARE(syncData["id"].toString(), expectedUsernameId);
        QCOMPARE(syncData["experiments"].toString().length(), 12761);
    }
}

void QtInstagramTest::testBodilessRequests_data()
{
    QTest::addColumn<TestedMethod>("method");
    QTest::addColumn<QUrl>("expectedUrl");
    QTest::addColumn<QString>("expectedSignal");

    // media endpoint

    QTest::newRow("getLikedFeed, no max") <<
        TestedMethod([](Instagram &i) { i.getLikedFeed(); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/liked/") <<
        SIGNAL(likedFeedDataReady(QVariant));

    QTest::newRow("getLikedFeed, with max") <<
        TestedMethod([](Instagram &i) { i.getLikedFeed("999"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/liked/?max_id=999") <<
        SIGNAL(likedFeedDataReady(QVariant));

    QTest::newRow("getFriendship") <<
        TestedMethod([](Instagram &i) { i.getFriendship("tom"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/show/tom/") <<
        SIGNAL(friendshipDataReady(QVariant));

    QTest::newRow("getRecentActivityInbox") <<
        TestedMethod([](Instagram &i) { i.getRecentActivityInbox(); }) <<
        QUrl("https://i.instagram.com/api/v1/news/inbox/?"
             "activity_module=all&show_su=true") <<
        SIGNAL(recentActivityInboxDataReady(QVariant));

    QTest::newRow("getComments, no max") <<
        TestedMethod([](Instagram &i) { i.getComments("321"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/321/comments/?"
             "ig_sig_key_version=4") <<
        SIGNAL(mediaCommentsDataReady(QVariant));

    QTest::newRow("getComments, with max") <<
        TestedMethod([](Instagram &i) { i.getComments("321", "789"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/321/comments/?"
             "ig_sig_key_version=4&max_id=789") <<
        SIGNAL(mediaCommentsDataReady(QVariant));

    QTest::newRow("getLikedMedia, no max") <<
        TestedMethod([](Instagram &i) { i.getLikedMedia(); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/liked/") <<
        SIGNAL(likedMediaDataReady(QVariant));

    QTest::newRow("getLikedMedia, with max") <<
        TestedMethod([](Instagram &i) { i.getLikedMedia("12"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/liked/?max_id=12") <<
        SIGNAL(likedMediaDataReady(QVariant));

    QTest::newRow("getMediaLikers") <<
        TestedMethod([](Instagram &i) { i.getMediaLikers("3232"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/3232/likers/") <<
        SIGNAL(mediaLikersDataReady(QVariant));

    // people endpoint

    QTest::newRow("getInfoByName") <<
        TestedMethod([](Instagram &i) { i.getInfoByName("tom_brown"); }) <<
        QUrl("https://i.instagram.com/api/v1/users/tom_brown/usernameinfo/") <<
        SIGNAL(infoByNameDataReady(QVariant));

    QTest::newRow("getFollowingRecentActivity") <<
        TestedMethod([](Instagram &i) { i.getFollowingRecentActivity(); }) <<
        QUrl("https://i.instagram.com/api/v1/news/?") <<
        SIGNAL(followingRecentActivityDataReady(QVariant));

    QTest::newRow("getFollowing, no params") <<
        TestedMethod([](Instagram &i) { i.getFollowing("john"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/following/?"
             "rank_token=RANKTOKEN") <<
        SIGNAL(followingDataReady(QVariant));


    QTest::newRow("getFollowing, with max") <<
        TestedMethod([](Instagram &i) { i.getFollowing("john", "45"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/following/?"
             "rank_token=RANKTOKEN&max_id=45") <<
        SIGNAL(followingDataReady(QVariant));

    QTest::newRow("getFollowing, with query") <<
        TestedMethod([](Instagram &i) { i.getFollowing("john", QString(), "search_query"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/following/?"
             "rank_token=RANKTOKEN&query=search_query") <<
        SIGNAL(followingDataReady(QVariant));

    QTest::newRow("getFollowing, with max and query") <<
        TestedMethod([](Instagram &i) { i.getFollowing("john", "981", "search_query2"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/following/?"
             "rank_token=RANKTOKEN&max_id=981&query=search_query2") <<
        SIGNAL(followingDataReady(QVariant));

    QTest::newRow("getFollowers, no params") <<
        TestedMethod([](Instagram &i) { i.getFollowers("john"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/followers/?"
             "rank_token=RANKTOKEN") <<
        SIGNAL(followersDataReady(QVariant));

    QTest::newRow("getFollowers, with max") <<
        TestedMethod([](Instagram &i) { i.getFollowers("john", "45"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/followers/?"
             "rank_token=RANKTOKEN&max_id=45") <<
        SIGNAL(followersDataReady(QVariant));

    QTest::newRow("getFollowers, with query") <<
        TestedMethod([](Instagram &i) { i.getFollowers("john", QString(), "search_query"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/followers/?"
             "rank_token=RANKTOKEN&query=search_query") <<
        SIGNAL(followersDataReady(QVariant));

    QTest::newRow("getFollowers, with max and query") <<
        TestedMethod([](Instagram &i) { i.getFollowers("john", "981", "search_query2"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/john/followers/?"
             "rank_token=RANKTOKEN&max_id=981&query=search_query2") <<
        SIGNAL(followersDataReady(QVariant));
}

void QtInstagramTest::testBodilessRequests()
{
    QFETCH(TestedMethod, method);
    QFETCH(QUrl, expectedUrl);
    QFETCH(QString, expectedSignal);

    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    method(instagram);
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    /* Replace dynamic parameters */
    QString expectedUrlString(expectedUrl.toString().replace("RANKTOKEN", m_rankToken));
    QCOMPARE(reply->url(), QUrl(expectedUrlString));

    QCOMPARE(reply->headers(), m_defaultHeaders);
    QVERIFY(reply->body().isEmpty());

    sendResponseAndCheckSignal(&instagram, reply,
                               expectedSignal.toUtf8().constData());
}

void QtInstagramTest::testLike()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.like("123", "random");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/123/like/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
        { "media_id", "123" },
        { "module_name", "random" },
        { "radio-type", "wifi-none" },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(likeDataReady(QVariant)));
}

void QtInstagramTest::testUnlike()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.unLike("567", "images");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/567/unlike/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
        { "media_id", "567" },
        { "module_name", "images" },
        { "radio-type", "wifi-none" },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(unLikeDataReady(QVariant)));
}

void QtInstagramTest::testGetInfoMedia()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.getInfoMedia("121");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/121/info/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
        { "media_id", "121" },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(mediaInfoReady(QVariant)));
}

void QtInstagramTest::testDeleteMedia()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.deleteMedia("135", "image");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/135/delete/?media_type=image"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
        { "media_id", "135" },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(mediaDeleted(QVariant)));
}

void QtInstagramTest::testEditMedia()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.editMedia("987", "New caption", "image");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/987/edit_media/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
        { "caption_text", "New caption" },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(mediaEdited(QVariant)));
}

void QtInstagramTest::testComment_data()
{
    QTest::addColumn<QString>("replyId");

    QTest::newRow("no reply ID") <<
        QString();

    QTest::newRow("with reply") <<
        QString("@1020");
}

void QtInstagramTest::testComment()
{
    QFETCH(QString, replyId);

    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.comment("585", "Hi there!", replyId, "spam");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/585/comment/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);

    QJsonObject request = extractSignedData(reply->body());
    QVERIFY(checkUuid(request["idempotence_token"].toString()));
    QCOMPARE(request["_uuid"].toString(), m_uuid);
    QCOMPARE(request["_csrftoken"].toString(), QString("Set-Cookie: csrftoken=abc"));
    QCOMPARE(request["_uid"].toString(), m_usernameId);
    QCOMPARE(request["containermoudle"].toString(), QString("spam"));
    QCOMPARE(request["comment_text"].toString(), QString("Hi there!"));
    QCOMPARE(request["radio-type"].toString(), QString("wifi-none"));
    if (replyId.isEmpty()) {
        QVERIFY(!request.contains("replied_to_comment_id"));
    } else {
        QCOMPARE(request["replied_to_comment_id"].toString(), replyId);
    }

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(commentPosted(QVariant)));
}

void QtInstagramTest::testDeleteComment()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.deleteComment("567", "12345");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/567/comment/12345/delete/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(commentDeleted(QVariant)));
}

void QtInstagramTest::testLikeComment()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.likeComment("8765");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/8765/comment_like/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(commentLiked(QVariant)));
}

void QtInstagramTest::testUnlikeComment()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.unlikeComment("5555");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(), QUrl("https://i.instagram.com/api/v1/media/5555/comment_unlike/"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QJsonObject expectedRequest {
        { "_csrftoken", "Set-Cookie: csrftoken=abc" },
        { "_uid", m_usernameId },
        { "_uuid", m_uuid },
    };
    QCOMPARE(extractSignedData(reply->body()), expectedRequest);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(commentUnliked(QVariant)));
}

void QtInstagramTest::testGetInfoById()
{
    Instagram instagram;

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.getInfoById("1234");
    QTRY_COMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url().toString(QUrl::RemoveQuery),
             QString("https://i.instagram.com/api/v1/users/1234/info/"));
    QUrlQuery query(reply->url());
    QVERIFY(query.hasQueryItem("device_id"));
    QCOMPARE(reply->headers(), m_defaultHeaders);
    QVERIFY(reply->body().isEmpty());

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(infoByIdDataReady(QVariant)));
}

QTEST_GUILESS_MAIN(QtInstagramTest)

#include "tst_instagram.moc"
