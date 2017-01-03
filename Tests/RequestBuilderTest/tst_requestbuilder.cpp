#include "tst_global.h"

class RequestBuilderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testBuilding_data();
	void testBuilding();

	void testSending_data();
	void testSending();

private:
	QNetworkAccessManager *nam;
};

void RequestBuilderTest::initTestCase()
{
	initTestJsonServer();
	nam = new QNetworkAccessManager(this);
}

void RequestBuilderTest::cleanupTestCase()
{
	nam->deleteLater();
	nam = nullptr;
}

void RequestBuilderTest::testBuilding_data()
{
	QTest::addColumn<QUrl>("base");
	QTest::addColumn<QString>("user");
	QTest::addColumn<QString>("pass");
	QTest::addColumn<QVersionNumber>("version");
	QTest::addColumn<QtRestClient::HeaderHash>("headers");
	QTest::addColumn<QUrlQuery>("params");
	QTest::addColumn<QString>("fragment");
	QTest::addColumn<QString>("path");
	QTest::addColumn<bool>("trailingSlash");
	QTest::addColumn<QNetworkRequest::Attribute>("attributeKey");
	QTest::addColumn<QVariant>("attributeValue");
	QTest::addColumn<QSslConfiguration>("sslConfig");
	QTest::addColumn<QUrl>("resultUrl");

	QTest::newRow("base") << QUrl("https://api.example.com/basic/")
						  << QString()
						  << QString()
						  << QVersionNumber()
						  << QtRestClient::HeaderHash()
						  << QUrlQuery()
						  << QString()
						  << QString()
						  << false
						  << (QNetworkRequest::Attribute)0
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic");

	QTest::newRow("credentials") << QUrl("https://api.example.com/basic/")
								 << "user"
								 << "password"
								 << QVersionNumber()
								 << QtRestClient::HeaderHash()
								 << QUrlQuery()
								 << QString()
								 << QString()
								 << false
								 << (QNetworkRequest::Attribute)0
								 << QVariant()
								 << QSslConfiguration::defaultConfiguration()
								 << QUrl("https://user:password@api.example.com/basic");

	QTest::newRow("version") << QUrl("https://api.example.com/basic/")
							 << QString()
							 << QString()
							 << QVersionNumber(4,2,0)
							 << QtRestClient::HeaderHash()
							 << QUrlQuery()
							 << QString()
							 << QString()
							 << false
							 << (QNetworkRequest::Attribute)0
							 << QVariant()
							 << QSslConfiguration::defaultConfiguration()
							 << QUrl("https://api.example.com/basic/v4.2");

	QTest::newRow("header") << QUrl("https://api.example.com/basic/")
							<< QString()
							<< QString()
							<< QVersionNumber()
							<< QtRestClient::HeaderHash({{"Bearer", "Secret"}})
							<< QUrlQuery()
							<< QString()
							<< QString()
							<< false
							<< (QNetworkRequest::Attribute)0
							<< QVariant()
							<< QSslConfiguration::defaultConfiguration()
							<< QUrl("https://api.example.com/basic");

	QUrlQuery query;
	query.addQueryItem("p1", "baum");
	query.addQueryItem("p2", "42");
	QTest::newRow("parameters") << QUrl("https://api.example.com/basic/")
								<< QString()
								<< QString()
								<< QVersionNumber()
								<< QtRestClient::HeaderHash()
								<< query
								<< QString()
								<< QString()
								<< false
								<< (QNetworkRequest::Attribute)0
								<< QVariant()
								<< QSslConfiguration::defaultConfiguration()
								<< QUrl("https://api.example.com/basic?p1=baum&p2=42");

	QTest::newRow("fragment") << QUrl("https://api.example.com/basic/")
							  << QString()
							  << QString()
							  << QVersionNumber()
							  << QtRestClient::HeaderHash()
							  << QUrlQuery()
							  << QStringLiteral("example")
							  << QString()
							  << false
							  << (QNetworkRequest::Attribute)0
							  << QVariant()
							  << QSslConfiguration::defaultConfiguration()
							  << QUrl("https://api.example.com/basic#example");

	QTest::newRow("path") << QUrl("https://api.example.com/basic/")
						  << QString()
						  << QString()
						  << QVersionNumber()
						  << QtRestClient::HeaderHash()
						  << QUrlQuery()
						  << QString()
						  << QStringLiteral("/examples/exampleStuff/")
						  << false
						  << (QNetworkRequest::Attribute)0
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic/examples/exampleStuff");

	QTest::newRow("slash") << QUrl("https://api.example.com/basic/")
						   << QString()
						   << QString()
						   << QVersionNumber()
						   << QtRestClient::HeaderHash()
						   << QUrlQuery()
						   << QString()
						   << QString()
						   << true
						   << (QNetworkRequest::Attribute)0
						   << QVariant()
						   << QSslConfiguration::defaultConfiguration()
						   << QUrl("https://api.example.com/basic/");

	QTest::newRow("attribute") << QUrl("https://api.example.com/basic/")
							   << QString()
							   << QString()
							   << QVersionNumber()
							   << QtRestClient::HeaderHash()
							   << QUrlQuery()
							   << QString()
							   << QString()
							   << false
							   << QNetworkRequest::CacheSaveControlAttribute
							   << QVariant(false)
							   << QSslConfiguration::defaultConfiguration()
							   << QUrl("https://api.example.com/basic");

	auto config = QSslConfiguration::defaultConfiguration();
	config.setProtocol(QSsl::TlsV1_2);
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	QTest::newRow("sslConfig") << QUrl("https://api.example.com/basic/")
							   << QString()
							   << QString()
							   << QVersionNumber()
							   << QtRestClient::HeaderHash()
							   << QUrlQuery()
							   << QString()
							   << QString()
							   << false
							   << (QNetworkRequest::Attribute)0
							   << QVariant()
							   << config
							   << QUrl("https://api.example.com/basic");

	QTest::newRow("full") << QUrl("https://api.example.com/basic/")
						  << "user"
						  << "password"
						  << QVersionNumber(4,2,0)
						  << QtRestClient::HeaderHash({{"Bearer", "Secret"}})
						  << query
						  << QStringLiteral("example")
						  << QStringLiteral("/examples/exampleStuff")
						  << true
						  << QNetworkRequest::CacheSaveControlAttribute
						  << QVariant(false)
						  << config
						  << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff/?p1=baum&p2=42#example");
}

void RequestBuilderTest::testBuilding()
{
	QFETCH(QUrl, base);
	QFETCH(QString, user);
	QFETCH(QString, pass);
	QFETCH(QVersionNumber, version);
	QFETCH(QtRestClient::HeaderHash, headers);
	QFETCH(QUrlQuery, params);
	QFETCH(QString, fragment);
	QFETCH(QString, path);
	QFETCH(bool, trailingSlash);
	QFETCH(QNetworkRequest::Attribute, attributeKey);
	QFETCH(QVariant, attributeValue);
	QFETCH(QSslConfiguration, sslConfig);
	QFETCH(QUrl, resultUrl);

	auto builder = QtRestClient::RequestBuilder(nullptr, base)
				   .setCredentials(user, pass)
				   .setVersion(version)
				   .addPath(path)
				   .addParameters(params)
				   .setFragment(fragment)
				   .addHeaders(headers)
				   .setSslConfig(sslConfig);
	if(trailingSlash)
		builder.trailingSlash();
	if(attributeValue.isValid())
		builder.setAttribute(attributeKey, attributeValue);

	auto request = builder.build();

	QCOMPARE(request.url(), resultUrl);
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		QCOMPARE(request.rawHeader(it.key()), it.value());
	QCOMPARE(request.sslConfiguration(), sslConfig);
	if(attributeValue.isValid())
		QCOMPARE(request.attribute(attributeKey), attributeValue);
}

void RequestBuilderTest::testSending_data()
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
	QTest::newRow("testDefaultGet") << QUrl("http://localhost:3000/posts/1")
									<< QJsonObject()
									<< QByteArray()
									<< 200
									<< QNetworkReply::NoError
									<< object;

	object["title"] = "baum";
	object["body"] = 42;
	QTest::newRow("testPut") << QUrl("http://localhost:3000/posts/1")
							 << object
							 << QByteArray("PUT")
							 << 200
							 << QNetworkReply::NoError
							 << object;

	QTest::newRow("testError") << QUrl("http://localhost:3000/posts/baum")
									<< QJsonObject()
									<< QByteArray("GET")
									<< 404
									<< QNetworkReply::ContentNotFoundError
									<< QJsonObject();
}

void RequestBuilderTest::testSending()
{
	QFETCH(QUrl, url);
	QFETCH(QJsonObject, body);
	QFETCH(QByteArray, verb);
	QFETCH(int, status);
	QFETCH(QNetworkReply::NetworkError, error);
	QFETCH(QJsonObject, object);

	auto builder = QtRestClient::RequestBuilder(nam, url);
	if(!verb.isEmpty())
		builder.setVerb(verb);
	if(!body.isEmpty())
		builder.setBody(body);

	auto reply = builder.send();
	QSignalSpy replySpy(reply, &QNetworkReply::finished);

	QVERIFY(replySpy.wait());
	QCOMPARE(reply->error(), error);
	QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), status);

	QJsonParseError e;
	auto repData = QJsonDocument::fromJson(reply->readAll(), &e).object();
	QCOMPARE(e.error, QJsonParseError::NoError);
	QCOMPARE(repData, object);

	reply->deleteLater();
}

QTEST_MAIN(RequestBuilderTest)

#include "tst_requestbuilder.moc"
