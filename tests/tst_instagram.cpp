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
#include <QUrlQuery>
#include "fake_network.h"

class QtInstagramTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testLogin_data();
    void testLogin();

private:
    QString extractUuid(const QUrl &url) const;
    QJsonObject extractSignedData(const QByteArray &body) const;

private:
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

QTEST_GUILESS_MAIN(QtInstagramTest)

#include "tst_instagram.moc"
