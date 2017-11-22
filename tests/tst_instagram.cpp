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

namespace QTest {
template<>
char *toString(const QJsonObject &p)
{
    QJsonDocument doc(p);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);
    return qstrdup(ba.data());
}
} // namespace

class QtInstagramTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testLogin_data();
    void testLogin();

    void testBodilessRequests_data();
    void testBodilessRequests();
    void testRequests_data();
    void testRequests();

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

void QtInstagramTest::testRequests_data()
{
    QTest::addColumn<TestedMethod>("method");
    QTest::addColumn<QUrl>("expectedUrl");
    QTest::addColumn<QJsonObject>("expectedBody");
    QTest::addColumn<QString>("expectedSignal");

    // media endpoint

    QTest::newRow("like") <<
        TestedMethod([](Instagram &i) { i.like("123", "random"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/123/like/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "media_id", "123" },
            { "module_name", "random" },
            { "radio-type", "wifi-none" },
        } <<
        SIGNAL(likeDataReady(QVariant));

    QTest::newRow("unLike") <<
        TestedMethod([](Instagram &i) { i.unLike("567", "images"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/567/unlike/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "media_id", "567" },
            { "module_name", "images" },
            { "radio-type", "wifi-none" },
        } <<
        SIGNAL(unLikeDataReady(QVariant));

    QTest::newRow("getInfoMedia") <<
        TestedMethod([](Instagram &i) { i.getInfoMedia("121"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/121/info/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "media_id", "121" },
        } <<
        SIGNAL(mediaInfoReady(QVariant));

    QTest::newRow("deleteMedia") <<
        TestedMethod([](Instagram &i) { i.deleteMedia("135", "image"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/135/delete/?media_type=image") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "media_id", "135" },
        } <<
        SIGNAL(mediaDeleted(QVariant));

    QTest::newRow("editMedia") <<
        TestedMethod([](Instagram &i) { i.editMedia("987", "New caption", "image"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/987/edit_media/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "caption_text", "New caption" },
        } <<
        SIGNAL(mediaEdited(QVariant));

    QTest::newRow("comment, no reply ID") <<
        TestedMethod([](Instagram &i) { i.comment("585", "Hi there!", QString(), "spam"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/585/comment/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "containermoudle", "spam" },
            { "comment_text", "Hi there!" },
            { "radio-type", "wifi-none" },
            { "idempotence_token", "ANYUUID" },
        } <<
        SIGNAL(commentPosted(QVariant));

    QTest::newRow("comment, with reply") <<
        TestedMethod([](Instagram &i) { i.comment("585", "Hi there!", "@1020", "spam"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/585/comment/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "containermoudle", "spam" },
            { "comment_text", "Hi there!" },
            { "radio-type", "wifi-none" },
            { "idempotence_token", "ANYUUID" },
            { "replied_to_comment_id", "@1020" },
        } <<
        SIGNAL(commentPosted(QVariant));

    QTest::newRow("deleteComment") <<
        TestedMethod([](Instagram &i) { i.deleteComment("567", "12345"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/567/comment/12345/delete/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(commentDeleted(QVariant));

    QTest::newRow("likeComment") <<
        TestedMethod([](Instagram &i) { i.likeComment("8765"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/8765/comment_like/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(commentLiked(QVariant));

    QTest::newRow("unlikeComment") <<
        TestedMethod([](Instagram &i) { i.unlikeComment("5555"); }) <<
        QUrl("https://i.instagram.com/api/v1/media/5555/comment_unlike/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(commentUnliked(QVariant));
}

void QtInstagramTest::testRequests()
{
    QFETCH(TestedMethod, method);
    QFETCH(QUrl, expectedUrl);
    QFETCH(QJsonObject, expectedBody);
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

    /* Replace dynamic parameters in body */
    QJsonObject body = extractSignedData(reply->body());
    auto i = expectedBody.begin();
    while (i != expectedBody.end()) {
        QJsonValueRef v = i.value();
        if (!v.isString()) { i++; continue; }

        QString value = v.toString();
        if (value == "ANYUUID") {
            // just check that the UUID is valid, and remove it from the object
            QVERIFY(checkUuid(body[i.key()].toString()));
            body.remove(i.key());
            i = expectedBody.erase(i);
            continue;
        }
        v = value.
            replace("USERNAMEID", m_usernameId).
            replace("UUID", m_uuid);
        i++;
    }
    QCOMPARE(body, expectedBody);

    sendResponseAndCheckSignal(&instagram, reply,
                               expectedSignal.toUtf8().constData());
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
