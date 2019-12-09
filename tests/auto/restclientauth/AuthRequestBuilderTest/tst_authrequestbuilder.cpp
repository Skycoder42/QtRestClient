#include "testlib.h"
#include <QtRestClientAuth/QtRestClientAuth>
using namespace QtRestClient;
using namespace QtRestClient::Auth;

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
	QVERIFY(server->setupRoutes());
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
	QTest::addColumn<BodyType>("body");
	QTest::addColumn<QByteArray>("verb");
	QTest::addColumn<int>("status");
	QTest::addColumn<QNetworkReply::NetworkError>("error");
	QTest::addColumn<BodyType>("object");

	QCborMap map;
	map[QStringLiteral("id")] = 1;
	map[QStringLiteral("userId")] = 1;
	map[QStringLiteral("title")] = QStringLiteral("Title1");
	map[QStringLiteral("body")] = QStringLiteral("Body1");

	QTest::newRow("testDefaultGet.cbor") << server->url("/posts/1")
										 << Testlib::CBody()
										 << QByteArray()
										 << 200
										 << QNetworkReply::NoError
										 << Testlib::CBody(map);
	QTest::newRow("testDefaultGet.json") << server->url("/posts/1")
										 << Testlib::JBody()
										 << QByteArray()
										 << 200
										 << QNetworkReply::NoError
										 << Testlib::JBody(map);

	map[QStringLiteral("title")] = "baum";
	map.remove(QStringLiteral("body")); // TODO workarounb
	map[QStringLiteral("body")] = 42;
	QTest::newRow("testPut.cbor") << server->url("/posts/1")
								  << Testlib::CBody(map)
								  << QByteArray("PUT")
								  << 200
								  << QNetworkReply::NoError
								  << Testlib::CBody(map);
	QTest::newRow("testPut.json") << server->url("/posts/1")
								  << Testlib::JBody(map)
								  << QByteArray("PUT")
								  << 200
								  << QNetworkReply::NoError
								  << Testlib::JBody(map);

	QTest::newRow("testError.cbor") << server->url("/posts/3434")
									<< Testlib::CBody()
									<< QByteArray("GET")
									<< 404
									<< QNetworkReply::ContentNotFoundError
									<< Testlib::CBody();
	QTest::newRow("testError.json") << server->url("/posts/3434")
									<< Testlib::JBody()
									<< QByteArray("GET")
									<< 404
									<< QNetworkReply::ContentNotFoundError
									<< Testlib::JBody();
}

void AuthRequestBuilderTest::testSending()
{
	QFETCH(QUrl, url);
	QFETCH(BodyType, body);
	QFETCH(QByteArray, verb);
	QFETCH(int, status);
	QFETCH(QNetworkReply::NetworkError, error);
	QFETCH(BodyType, object);

	AuthRequestBuilder builder(url, oAuth);
	builder.setAccept(body.accept());
	builder.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	if (!verb.isEmpty())
		builder.setVerb(verb);
	if (body.isValid()) {
		body.visit([&](const auto &data) {
			builder.setBody(data);
		});
	}

	auto reply = builder.send();
	QSignalSpy replySpy(reply, &QNetworkReply::finished);

	QVERIFY(replySpy.wait());
	QCOMPARE(reply->error(), error);
	QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), status);

	if (error == QNetworkReply::NoError) {
		auto repData = BodyType::parse(reply);
		QVERIFY(repData.isValid());
		QCOMPARE(repData, object);
	}

	reply->deleteLater();
}

QTEST_MAIN(AuthRequestBuilderTest)

#include "tst_authrequestbuilder.moc"
