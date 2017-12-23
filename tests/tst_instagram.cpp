#include "QtInstagram"

#include <QDebug>
#include <QJsonArray>
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
    void testPostImage_data();
    void testPostImage();

    void testBodilessRequests_data();
    void testBodilessRequests();
    void testRequests_data();
    void testRequests();

private:
    QString extractUuid(const QUrl &url) const;
    QJsonObject extractSignedData(const QByteArray &body) const;
    void doLogin(Instagram *instagram);
    void sendResponseAndCheckSignal(Instagram *instagram, FakeReply *reply,
                                    const char *signalName);
    bool checkUuid(const QString &uuid) const;
    bool checkDateTime(const QString &string) const;
    void checkUploadRequest(FakeReply *request, const QString &filePath,
                            const QString &uploadId = QString());
    QString &replaceMacros(QString source) const;
    void replaceMacros(QJsonObject &data, QJsonObject &expectedData) const;

private:
    QString m_deviceId;
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

    QJsonObject loginData = extractSignedData(reply->body());
    m_deviceId = loginData["device_id"].toString();
    QVERIFY(!m_deviceId.isEmpty());

    // Sent the login reply
    reply->setData("{\"logged_in_user\": {\"pk\": 789000123}}");
    QTRY_COMPARE(profileConnected.count(), 1);
}

bool QtInstagramTest::checkUuid(const QString &uuid) const
{
    return !QUuid(uuid).isNull();
}

bool QtInstagramTest::checkDateTime(const QString &string) const
{
    QDateTime dateTime = QDateTime::fromString(string, "yyyy:MM:dd HH:mm:ss");
    return dateTime.isValid();
}

void QtInstagramTest::checkUploadRequest(FakeReply *request,
                                         const QString &filePath,
                                         const QString &uploadId)
{
    QJsonObject headers = request->headers();
    QString contentType = headers["Content-Type"].toString();

    const QString contentTypeStart("multipart/form-data; boundary=");
    QCOMPARE(contentType.left(contentTypeStart.length()), contentTypeStart);

    const QString boundary = contentType.mid(contentTypeStart.length());
    qDebug() << "boundary" << boundary;
    QByteArray boundaryLine("--" + boundary.toUtf8() + "\r\n");

    int offset = 0;
    QByteArray body = request->body();
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    QVERIFY(false);
}

QString &QtInstagramTest::replaceMacros(QString source) const
{
    return source.
        replace("DEVICEID", m_deviceId).
        replace("RANKTOKEN", m_rankToken).
        replace("USERNAMEID", m_usernameId).
        replace("UUID", m_uuid);
}

void QtInstagramTest::replaceMacros(QJsonObject &data, QJsonObject &expectedData) const
{
    auto i = expectedData.begin();
    while (i != expectedData.end()) {
        QJsonValueRef v = i.value();
        if (!v.isString()) { i++; continue; }

        QString value = v.toString();
        bool mustRemove = false;
        if (value == "ANYUUID") {
            // just check that the UUID is valid, and remove it from the object
            if (checkUuid(data[i.key()].toString())) {
                mustRemove = true;
            }
        } else if (value == "DATETIME") {
            if (checkDateTime(data[i.key()].toString())) {
                mustRemove = true;
            }
        }
        if (mustRemove) {
            data.remove(i.key());
            i = expectedData.erase(i);
            continue;
        }
        v = replaceMacros(value);
        i++;
    }
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

void QtInstagramTest::testPostImage_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("uploadId");
    QTest::addColumn<QString>("uploadReply");
    QTest::addColumn<QJsonObject>("expectedConfigureBody");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("missing file") <<
        "/tmp/this_file_does_not_exist.jpg" <<
        "aabbcc" <<
        "" <<
        QJsonObject {} <<
        "Image not found";

    QTest::newRow("rect image, server failure") <<
        DATA_DIR "/rect.jpg" <<
        "aabbcc" <<
        "{\"status\": \"fail\", \"message\": \"Bad luck\"}" <<
        QJsonObject {} <<
        QString("Bad luck");

    QTest::newRow("rect image, wrong server reply") <<
        DATA_DIR "/rect.jpg" <<
        "aabbcc" <<
        "{\"status\": \"ok\"}" <<
        QJsonObject {} <<
        QString("Wrong UPLOAD_ID:");

    QTest::newRow("rect image, success") <<
        DATA_DIR "/rect.jpg" <<
        "aabbcc" <<
        "{\"status\": \"ok\", \"upload_id\": \"123\"}" <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "upload_id", "123" },
            { "camera_model", "HM1S" },
            { "source_type", 3 },
            { "date_time_original", "DATETIME" },
            { "camera_make", "XIAOMI" },
            { "caption", "This is the caption" },
            { "device", QJsonObject {
                { "manufacturer", "Xiaomi" },
                { "model", "HM 1SW" },
                { "android_version", 18 },
                { "android_release", "4.3" },
            } },
            { "edits", QJsonObject {
                { "crop_original_size", QJsonArray { 600, 400 } },
                { "crop_zoom", 1.3333334 },
                { "crop_center", QJsonArray { 0.0, -0.0 } },
            } },
            { "extra", QJsonObject {
                { "source_width", 600 },
                { "source_height", 400 },
            } },
        } <<
        QString();
}

void QtInstagramTest::testPostImage()
{
    QFETCH(QString, filePath);
    QFETCH(QString, uploadId);
    QFETCH(QString, uploadReply);
    QFETCH(QJsonObject, expectedConfigureBody);
    QFETCH(QString, expectedError);

    Instagram instagram;
    QSignalSpy error(&instagram, &Instagram::error);

    FakeNam dummyNam;
    instagram.setNetworkAccessManager(&dummyNam);
    doLogin(&instagram);

    QSignalSpy requestCreated(&dummyNam, &FakeNam::requestCreated);

    instagram.postImage(filePath, "This is the caption", uploadId);
    if (requestCreated.isEmpty()) {
        QCOMPARE(error.count(), 1);
        QCOMPARE(error.at(0).at(0).toString(), expectedError);
        return;
    }

    QCOMPARE(requestCreated.count(), 1);
    FakeReply *reply = requestCreated.at(0).at(0).value<FakeReply*>();

    QCOMPARE(reply->url(),
             QUrl("https://i.instagram.com/api/v1/upload/photo/"));

    QJsonObject headers = reply->headers();
    QString contentType = headers["Content-Type"].toString();

    const QString contentTypeStart("multipart/form-data; boundary=");
    QCOMPARE(contentType.left(contentTypeStart.length()), contentTypeStart);

    const QString boundary = contentType.mid(contentTypeStart.length());
    QByteArray boundaryLine("--" + boundary.toUtf8() + "\r\n");

    QByteArray body = reply->body();
    QByteArray dataBlock;
    int offset = 0;

    // upload_id
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    dataBlock = "Content-Disposition: form-data; name=\"upload_id\"\r\n\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    if (uploadId.isEmpty()) {
        int uploadIdEnd = body.indexOf("\r\n", offset);
        uploadId = body.mid(offset, uploadIdEnd - offset);
    } else {
        dataBlock = uploadId.toUtf8() + "\r\n";
        QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
        offset += dataBlock.length();
    }

    // _uuid
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    dataBlock = "Content-Disposition: form-data; name=\"_uuid\"\r\n\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    dataBlock = m_uuid.toUtf8() + "\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    // _csrftoken
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    dataBlock = "Content-Disposition: form-data; name=\"_csrftoken\"\r\n\r\n"
        "abc\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    // image_compression
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    dataBlock = "Content-Disposition: form-data; name=\"image_compression\"\r\n\r\n"
        "{\"lib_name\":\"jt\",\"lib_version\":\"1.3.0\",\"quality\":\"70\"}\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    // photo
    QCOMPARE(body.mid(offset, boundaryLine.length()), boundaryLine);
    offset += boundaryLine.length();

    QFileInfo fileInfo(filePath);
    QByteArray ext = fileInfo.completeSuffix().toUtf8();
    dataBlock = "Content-Disposition: form-data; name=\"photo\"; "
        "filename=\"pending_media_" + uploadId.toUtf8() + "." + ext + "\"\r\n"
        "Content-Transfer-Encoding: binary\r\n"
        "Content-Type: application/octet-stream\r\n\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    dataBlock = file.readAll() + "\r\n";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    // end message
    dataBlock = "--" + boundary.toUtf8() + "--";
    QCOMPARE(body.mid(offset, dataBlock.length()), dataBlock);
    offset += dataBlock.length();

    QCOMPARE(body.length(), offset);

    // check headers
    int contentLength = headers["Content-Length"].toString().toInt();
    QCOMPARE(contentLength, body.length());

    headers.remove("Content-Length");
    headers.remove("Content-Type");
    QJsonObject expectedHeaders {
        { "Connection", "close" },
        { "Accept", "*/*" },
        { "User-Agent", "Instagram 10.33.0 Android (18/4.3; 320dpi; 720x1280; "
            "Xiaomi; HM 1SW; armani; qcom; en_US)" },
        { "Cookie2","$Version=1" },
        { "Accept-Language","en-US" },
        { "Accept-Encoding","gzip" },
    };
    QCOMPARE(headers, expectedHeaders);

    // send the reply
    reply->setData(uploadReply.toUtf8());
    if (!expectedError.isEmpty()) {
        QTRY_COMPARE(error.count(), 1);
        QCOMPARE(error.at(0).at(0).toString(), expectedError);
        return;
    }
    QTRY_COMPARE(requestCreated.count(), 2);
    reply = requestCreated.at(1).at(0).value<FakeReply*>();
    QCOMPARE(error.count(), 0);

    QCOMPARE(reply->url(),
             QUrl("https://i.instagram.com/api/v1/media/configure/"));
    QJsonObject configureBody = extractSignedData(reply->body());
    replaceMacros(configureBody, expectedConfigureBody);
    QCOMPARE(configureBody, expectedConfigureBody);

    sendResponseAndCheckSignal(&instagram, reply,
                               SIGNAL(imageConfigureDataReady(QVariant)));
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

    QTest::newRow("getInfoById") <<
        TestedMethod([](Instagram &i) { i.getInfoById("1234"); }) <<
        QUrl("https://i.instagram.com/api/v1/users/1234/info/?device_id=DEVICEID") <<
        SIGNAL(infoByIdDataReady(QVariant));

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

    QTest::newRow("searchUser") <<
        TestedMethod([](Instagram &i) { i.searchUser("search_query3"); }) <<
        QUrl("https://i.instagram.com/api/v1/users/search/?"
             "query=search_query3"
             "&is_typeahead=true"
             "&rank_token=RANKTOKEN"
             "&ig_sig_key_version=4") <<
        SIGNAL(searchUserDataReady(QVariant));

    QTest::newRow("getSugestedUser") <<
        TestedMethod([](Instagram &i) { i.getSugestedUser("821"); }) <<
        QUrl("https://i.instagram.com/api/v1/discover/chaining/?"
             "target_id=821") <<
        SIGNAL(suggestedUserDataReady(QVariant));

    // direct endpoint

    QTest::newRow("getInbox, no cursor") <<
        TestedMethod([](Instagram &i) { i.getInbox(); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_v2/inbox/?"
             "persistentBadging=true&"
             "use_unified_inbox=true") <<
        SIGNAL(inboxDataReady(QVariant));

    QTest::newRow("getInbox, with cursor") <<
        TestedMethod([](Instagram &i) { i.getInbox("main"); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_v2/inbox/?"
             "persistentBadging=true&"
             "use_unified_inbox=true&"
             "cursor=main") <<
        SIGNAL(inboxDataReady(QVariant));

    QTest::newRow("getPendingInbox") <<
        TestedMethod([](Instagram &i) { i.getPendingInbox(); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_v2/pending_inbox/"
             "persistentBadging=true&"
             "use_unified_inbox=true") <<
        SIGNAL(pendingInboxDataReady(QVariant));

    QTest::newRow("getDirectThread, no cursor") <<
        TestedMethod([](Instagram &i) { i.getDirectThread("123"); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_v2/threads/123/?"
             "use_unified_inbox=true") <<
        SIGNAL(directThreadDataReady(QVariant));

    QTest::newRow("getDirectThread, with cursor") <<
        TestedMethod([](Instagram &i) { i.getDirectThread("321", "main"); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_v2/threads/321/?"
             "use_unified_inbox=true&"
             "cursor=main") <<
        SIGNAL(directThreadDataReady(QVariant));

    QTest::newRow("getRecentRecipients") <<
        TestedMethod([](Instagram &i) { i.getRecentRecipients(); }) <<
        QUrl("https://i.instagram.com/api/v1/direct_share/recent_recipients/") <<
        SIGNAL(recentRecipientsDataReady(QVariant));

    // discover endpoint

    QTest::newRow("getExploreFeed, no max") <<
        TestedMethod([](Instagram &i) { i.getExploreFeed(QString(), "0"); }) <<
        QUrl("https://i.instagram.com/api/v1/discover/explore/?"
             "is_prefetch=0&"
             "is_from_promote=false&"
             "session_id=abc&"
             "module=explore_popular") <<
        SIGNAL(exploreFeedDataReady(QVariant));

    QTest::newRow("getExploreFeed, with max") <<
        TestedMethod([](Instagram &i) { i.getExploreFeed("12", "1"); }) <<
        QUrl("https://i.instagram.com/api/v1/discover/explore/?"
             "is_prefetch=1&"
             "is_from_promote=false&"
             "session_id=abc&"
             "module=explore_popular&"
             "max_id=12") <<
        SIGNAL(exploreFeedDataReady(QVariant));

    // hashtag endpoint

    QTest::newRow("getTagFeed, no max") <<
        TestedMethod([](Instagram &i) { i.getTagFeed("cats"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/tag/cats/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true") <<
        SIGNAL(tagFeedDataReady(QVariant));

    QTest::newRow("getTagFeed, with max") <<
        TestedMethod([](Instagram &i) { i.getTagFeed("dogs", "13"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/tag/dogs/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true&"
             "max_id=13") <<
        SIGNAL(tagFeedDataReady(QVariant));

    // story endpoint

    QTest::newRow("getReelsTrayFeed") <<
        TestedMethod([](Instagram &i) { i.getReelsTrayFeed(); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/reels_tray/") <<
        SIGNAL(reelsTrayFeedDataReady(QVariant));

    QTest::newRow("getUserReelsMediaFeed") <<
        TestedMethod([](Instagram &i) { i.getUserReelsMediaFeed("2374"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/user/2374/reel_media/") <<
        SIGNAL(userReelsMediaFeedDataReady(QVariant));

    // timeline endpoint

    QTest::newRow("getUserFeed, no max, no ts") <<
        TestedMethod([](Instagram &i) { i.getUserFeed("2392"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/user/2392/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true") <<
        SIGNAL(userFeedDataReady(QVariant));

    QTest::newRow("getUserFeed, max, no ts") <<
        TestedMethod([](Instagram &i) { i.getUserFeed("2392", "23"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/user/2392/?"
             "rank_token=RANKTOKEN&"
             "max_id=23&"
             "ranked_content=true") <<
        SIGNAL(userFeedDataReady(QVariant));

    QTest::newRow("getUserFeed, no max, ts") <<
        TestedMethod([](Instagram &i) { i.getUserFeed("2392", QString(), "112"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/user/2392/?"
             "rank_token=RANKTOKEN&"
             "min_timestamp=112&"
             "ranked_content=true") <<
        SIGNAL(userFeedDataReady(QVariant));

    QTest::newRow("getUserFeed, max, ts") <<
        TestedMethod([](Instagram &i) { i.getUserFeed("2392", "21", "111"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/user/2392/?"
             "rank_token=RANKTOKEN&"
             "max_id=21&"
             "min_timestamp=111&"
             "ranked_content=true") <<
        SIGNAL(userFeedDataReady(QVariant));

    // usertag endpoint

    QTest::newRow("getUserTags, no max, no ts") <<
        TestedMethod([](Instagram &i) { i.getUserTags("2392"); }) <<
        QUrl("https://i.instagram.com/api/v1/usertags/2392/feed/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true") <<
        SIGNAL(userTagsDataReady(QVariant));

    QTest::newRow("getUserTags, max, no ts") <<
        TestedMethod([](Instagram &i) { i.getUserTags("2392", "23"); }) <<
        QUrl("https://i.instagram.com/api/v1/usertags/2392/feed/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true&"
             "max_id=23") <<
        SIGNAL(userTagsDataReady(QVariant));

    QTest::newRow("getUserTags, no max, ts") <<
        TestedMethod([](Instagram &i) { i.getUserTags("2392", QString(), "112"); }) <<
        QUrl("https://i.instagram.com/api/v1/usertags/2392/feed/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true&"
             "min_timestamp=112") <<
        SIGNAL(userTagsDataReady(QVariant));

    QTest::newRow("getUserTags, max, ts") <<
        TestedMethod([](Instagram &i) { i.getUserTags("2392", "21", "111"); }) <<
        QUrl("https://i.instagram.com/api/v1/usertags/2392/feed/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=true&"
             "max_id=21&"
             "min_timestamp=111") <<
        SIGNAL(userTagsDataReady(QVariant));
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
    QString expectedUrlString(replaceMacros(expectedUrl.toString()));
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

    // people endpoint

    QTest::newRow("follow") <<
        TestedMethod([](Instagram &i) { i.follow("2732"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/create/2732/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "user_id", "2732" },
            { "radio_type", "wifi-none" },
        } <<
        SIGNAL(followDataReady(QVariant));

    QTest::newRow("unFollow") <<
        TestedMethod([](Instagram &i) { i.unFollow("2731"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/destroy/2731/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "user_id", "2731" },
            { "radio_type", "wifi-none" },
        } <<
        SIGNAL(unfollowDataReady(QVariant));

    QTest::newRow("favorite") <<
        TestedMethod([](Instagram &i) { i.favorite("2761"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/favorite/2761/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(favoriteDataReady(QVariant));

    QTest::newRow("unFavorite") <<
        TestedMethod([](Instagram &i) { i.unFavorite("2765"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/unfavorite/2765/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(unFavoriteDataReady(QVariant));

    QTest::newRow("block") <<
        TestedMethod([](Instagram &i) { i.block("1989"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/block/1989/?") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "user_id", "1989" },
        } <<
        SIGNAL(blockDataReady(QVariant));

    QTest::newRow("unBlock") <<
        TestedMethod([](Instagram &i) { i.unBlock("7373"); }) <<
        QUrl("https://i.instagram.com/api/v1/friendships/unblock/7373/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "user_id", "7373" },
        } <<
        SIGNAL(unBlockDataReady(QVariant));

    // account endpoint

    QTest::newRow("removeProfilePicture") <<
        TestedMethod([](Instagram &i) { i.removeProfilePicture(); }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/remove_profile_picture/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(profilePictureDeleted(QVariant));

    QTest::newRow("setPrivateAccount") <<
        TestedMethod([](Instagram &i) { i.setPrivateAccount(); }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/set_private/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(setProfilePrivate(QVariant));

    QTest::newRow("setPublicAccount") <<
        TestedMethod([](Instagram &i) { i.setPublicAccount(); }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/set_public/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(setProfilePublic(QVariant));

    QTest::newRow("getCurrentUser") <<
        TestedMethod([](Instagram &i) { i.getCurrentUser(); }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/current_user/?edit=true") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(currentUserDataReady(QVariant));

    QTest::newRow("editProfile, male") <<
        TestedMethod([](Instagram &i) {
            i.editProfile("http://mysite.me", "+390732123", "John",
                          "I was born long time ago", "john@mysite.me",
                          true);
        }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/edit_profile/?edit=true") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "external_url", "http://mysite.me" },
            { "phone_number", "+390732123" },
            { "username", "just_me" },
            { "first_name", "John" },
            { "biography", "I was born long time ago" },
            { "email", "john@mysite.me" },
            { "gender", "1" },
        } <<
        SIGNAL(editDataReady(QVariant));

    QTest::newRow("editProfile, female") <<
        TestedMethod([](Instagram &i) {
            i.editProfile("http://mysite.me", "+390732123", "Cathy",
                          "This goes\non two lines.", "cathy@mysite.me",
                          false);
        }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/edit_profile/?edit=true") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "external_url", "http://mysite.me" },
            { "phone_number", "+390732123" },
            { "username", "just_me" },
            { "first_name", "Cathy" },
            { "biography", "This goes\non two lines." },
            { "email", "cathy@mysite.me" },
            { "gender", "0" },
        } <<
        SIGNAL(editDataReady(QVariant));

    QTest::newRow("checkUsername") <<
        TestedMethod([](Instagram &i) { i.checkUsername("tom"); }) <<
        QUrl("https://i.instagram.com/api/v1/users/check_username/") <<
        QJsonObject {
            { "_csrftoken", "missing" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
            { "username", "tom" },
        } <<
        SIGNAL(usernameCheckDataReady(QVariant));

    QTest::newRow("createAccount") <<
        TestedMethod([](Instagram &i) {
            i.createAccount("david", "secr3t", "david@site.me");
        }) <<
        QUrl("https://i.instagram.com/api/v1/accounts/create/") <<
        QJsonObject {
            { "_csrftoken", "missing" },
            { "_uuid", "UUID" },
            { "username", "david" },
            { "password", "secr3t" },
            { "first_name", "" },
            { "guid", "UUID" },
            { "device_id", "DEVICEID" },
            { "email", "david@site.me" },
            { "force_sign_up_code", "" },
            { "qs_stamp", "" },
        } <<
        SIGNAL(createAccountDataReady(QVariant));

    // timeline endpoint

    QTest::newRow("getTimeLineFeed, no max") <<
        TestedMethod([](Instagram &i) { i.getTimelineFeed(); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/timeline/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=false&"
             "_uuid=UUID") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uuid", "UUID" },
            { "battery_level", "100" },
            { "is_prefetch", "0" },
        } <<
        SIGNAL(timelineFeedDataReady(QVariant));

    QTest::newRow("getTimeLineFeed, with max") <<
        TestedMethod([](Instagram &i) { i.getTimelineFeed("18"); }) <<
        QUrl("https://i.instagram.com/api/v1/feed/timeline/?"
             "rank_token=RANKTOKEN&"
             "ranked_content=false&"
             "_uuid=UUID&"
             "max_id=18") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uuid", "UUID" },
            { "battery_level", "100" },
            { "is_prefetch", "0" },
            { "max_id", "18" },
        } <<
        SIGNAL(timelineFeedDataReady(QVariant));

    // usertag endpoint

    QTest::newRow("removeSelftag") <<
        TestedMethod([](Instagram &i) { i.removeSelftag("127"); }) <<
        QUrl("https://i.instagram.com/api/v1/usertags/127/remove/") <<
        QJsonObject {
            { "_csrftoken", "Set-Cookie: csrftoken=abc" },
            { "_uid", "USERNAMEID" },
            { "_uuid", "UUID" },
        } <<
        SIGNAL(removeSelftagDone(QVariant));
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
    QString expectedUrlString(replaceMacros(expectedUrl.toString()));
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
        v = replaceMacros(value);
        i++;
    }
    QCOMPARE(body, expectedBody);

    sendResponseAndCheckSignal(&instagram, reply,
                               expectedSignal.toUtf8().constData());
}

QTEST_GUILESS_MAIN(QtInstagramTest)

#include "tst_instagram.moc"
