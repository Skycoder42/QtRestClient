#include "testlib.h"
#include <QtRestClientAuth/QtRestClientAuth>

class AuthRequestBuilderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testSending_data();
	void testSending();

private:
	HttpServer *server;
	QOAuth2AuthorizationCodeFlow *oAuth;
};

void AuthRequestBuilderTest::initTestCase()
{
	server = new HttpServer(this);
	server->verifyRunning();
	server->setDefaultData();
	oAuth = new QOAuth2AuthorizationCodeFlow{new QNetworkAccessManager{this}, this};
	oAuth->setToken(server->generateToken());
}

void AuthRequestBuilderTest::cleanupTestCase()
{
	server->deleteLater();
	server = nullptr;
	oAuth->deleteLater();
	oAuth = nullptr;
}

void AuthRequestBuilderTest::testSending_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<QJsonObject>("body");
	QTest::addColumn<QByteArray>("verb");
	QTest::addColumn<int>("status");
	QTest::addColumn<QNetworkReply::NetworkError>("error");
	QTest::addColumn<QJsonObject>("object");

	QJsonObject object;
	object["userId"] = 1;
	object["id"] = 1;
	object["title"] = "Title1";
	object["body"] = "Body1";
	QTest::newRow("testDefaultGet") << server->url("posts/1")
									<< QJsonObject()
									<< QByteArray()
									<< 200
									<< QNetworkReply::NoError
									<< object;

	object["title"] = "baum";
	object["body"] = 42;
	QTest::newRow("testPut") << server->url("posts/1")
							 << object
							 << QByteArray("PUT")
							 << 200
							 << QNetworkReply::NoError
							 << object;

	QTest::newRow("testError") << server->url("posts/baum")
							   << QJsonObject()
							   << QByteArray("GET")
							   << 404
							   << QNetworkReply::ContentNotFoundError
							   << QJsonObject();
}

void AuthRequestBuilderTest::testSending()
{
	QFETCH(QUrl, url);
	QFETCH(QJsonObject, body);
	QFETCH(QByteArray, verb);
	QFETCH(int, status);
	QFETCH(QNetworkReply::NetworkError, error);
	QFETCH(QJsonObject, object);

	QtRestClient::AuthRequestBuilder builder(url, oAuth);
	builder.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	if(!verb.isEmpty())
		builder.setVerb(verb);
	if(!body.isEmpty())
		builder.setBody(body);

	auto reply = builder.send();
	QSignalSpy replySpy(reply, &QNetworkReply::finished);

	QVERIFY(replySpy.wait());
	QCOMPARE(reply->error(), error);
	QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), status);

	if(error == QNetworkReply::NoError) {
		QJsonParseError e;
		auto repData = QJsonDocument::fromJson(reply->readAll(), &e).object();
		QCOMPARE(e.error, QJsonParseError::NoError);
		QCOMPARE(repData, object);
	}

	reply->deleteLater();
}

QTEST_MAIN(AuthRequestBuilderTest)

#include "tst_authrequestbuilder.moc"
