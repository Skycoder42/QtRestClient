#include "testlib.h"
using namespace QtRestClient;

class RequestBuilderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testBuilding_data();
	void testBuilding();

	void testBuildingRelative_data();
	void testBuildingRelative();

	void testSending_data();
	void testSending();
	void setPostParamsSending();
	void testAsyncSending();

private:
	HttpServer *server;
	QNetworkAccessManager *nam;
};

void RequestBuilderTest::initTestCase()
{
	server = new HttpServer(this);
	QVERIFY(server->setupRoutes());
	server->setDefaultData();
	nam = new QNetworkAccessManager(this);
}

void RequestBuilderTest::cleanupTestCase()
{
	server->deleteLater();
	server = nullptr;
	nam->deleteLater();
	nam = nullptr;
}

void RequestBuilderTest::testBuilding_data()
{
	QTest::addColumn<QUrl>("base");
	QTest::addColumn<QString>("user");
	QTest::addColumn<QString>("pass");
	QTest::addColumn<QVersionNumber>("version");
	QTest::addColumn<HeaderHash>("headers");
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
						  << HeaderHash()
						  << QUrlQuery()
						  << QString()
						  << QString()
						  << false
						  << static_cast<QNetworkRequest::Attribute>(0)
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic");

	QTest::newRow("credentials") << QUrl("https://api.example.com/basic/")
								 << "user"
								 << "password"
								 << QVersionNumber()
								 << HeaderHash()
								 << QUrlQuery()
								 << QString()
								 << QString()
								 << false
								 << static_cast<QNetworkRequest::Attribute>(0)
								 << QVariant()
								 << QSslConfiguration::defaultConfiguration()
								 << QUrl("https://user:password@api.example.com/basic");

	QTest::newRow("version") << QUrl("https://api.example.com/basic/")
							 << QString()
							 << QString()
							 << QVersionNumber(4,2,0)
							 << HeaderHash()
							 << QUrlQuery()
							 << QString()
							 << QString()
							 << false
							 << static_cast<QNetworkRequest::Attribute>(0)
							 << QVariant()
							 << QSslConfiguration::defaultConfiguration()
							 << QUrl("https://api.example.com/basic/v4.2");

	QTest::newRow("header") << QUrl("https://api.example.com/basic/")
							<< QString()
							<< QString()
							<< QVersionNumber()
							<< HeaderHash({{"Bearer", "Secret"}})
							<< QUrlQuery()
							<< QString()
							<< QString()
							<< false
							<< static_cast<QNetworkRequest::Attribute>(0)
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
								<< HeaderHash()
								<< query
								<< QString()
								<< QString()
								<< false
								<< static_cast<QNetworkRequest::Attribute>(0)
								<< QVariant()
								<< QSslConfiguration::defaultConfiguration()
								<< QUrl("https://api.example.com/basic?p1=baum&p2=42");

	QTest::newRow("fragment") << QUrl("https://api.example.com/basic/")
							  << QString()
							  << QString()
							  << QVersionNumber()
							  << HeaderHash()
							  << QUrlQuery()
							  << QStringLiteral("example")
							  << QString()
							  << false
							  << static_cast<QNetworkRequest::Attribute>(0)
							  << QVariant()
							  << QSslConfiguration::defaultConfiguration()
							  << QUrl("https://api.example.com/basic#example");

	QTest::newRow("path") << QUrl("https://api.example.com/basic/")
						  << QString()
						  << QString()
						  << QVersionNumber()
						  << HeaderHash()
						  << QUrlQuery()
						  << QString()
						  << QStringLiteral("/examples/exampleStuff/")
						  << false
						  << static_cast<QNetworkRequest::Attribute>(0)
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic/examples/exampleStuff");

	QTest::newRow("slash") << QUrl("https://api.example.com/basic/")
						   << QString()
						   << QString()
						   << QVersionNumber()
						   << HeaderHash()
						   << QUrlQuery()
						   << QString()
						   << QString()
						   << true
						   << static_cast<QNetworkRequest::Attribute>(0)
						   << QVariant()
						   << QSslConfiguration::defaultConfiguration()
						   << QUrl("https://api.example.com/basic/");

	QTest::newRow("attribute") << QUrl("https://api.example.com/basic/")
							   << QString()
							   << QString()
							   << QVersionNumber()
							   << HeaderHash()
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
							   << HeaderHash()
							   << QUrlQuery()
							   << QString()
							   << QString()
							   << false
							   << static_cast<QNetworkRequest::Attribute>(0)
							   << QVariant()
							   << config
							   << QUrl("https://api.example.com/basic");

	QTest::newRow("full") << QUrl("https://api.example.com/basic/")
						  << "user"
						  << "password"
						  << QVersionNumber(4,2,0)
						  << HeaderHash({{"Bearer", "Secret"}})
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

	auto builder = RequestBuilder(base)
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

void RequestBuilderTest::testBuildingRelative_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<QUrl>("relative");
	QTest::addColumn<bool>("mergeQuery");
	QTest::addColumn<bool>("keepFragment");
	QTest::addColumn<QUrl>("resultUrl");

	QTest::newRow("currentDir") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
								<< QUrl(".")
								<< false
								<< false
								<< QUrl("https://user:password@api.example.com/basic/v4.2/examples");

	QTest::newRow("relativePath") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
								  << QUrl("../baum/42")
								  << false
								  << false
								  << QUrl("https://user:password@api.example.com/basic/v4.2/baum/42");

	QTest::newRow("keepQuery") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
							   << QUrl("./path")
							   << true
							   << false
							   << QUrl("https://user:password@api.example.com/basic/v4.2/examples/path?p1=baum&p2=42");

	QTest::newRow("customQuery") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
								 << QUrl("./path?baum=42")
								 << false
								 << false
								 << QUrl("https://user:password@api.example.com/basic/v4.2/examples/path?baum=42");

	QTest::newRow("mergedQuery") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
								 << QUrl("./path?baum=42")
								 << true
								 << false
								 << QUrl("https://user:password@api.example.com/basic/v4.2/examples/path?p1=baum&p2=42&baum=42");

	QTest::newRow("customFragment") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
									<< QUrl("./path#fragment")
									<< false
									<< false
									<< QUrl("https://user:password@api.example.com/basic/v4.2/examples/path#fragment");

	QTest::newRow("keepFragment") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
								  << QUrl("./path#fragment")
								  << false
								  << true
								  << QUrl("https://user:password@api.example.com/basic/v4.2/examples/path#example");

	QTest::newRow("newHost") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
							 << QUrl("//api.google.de/lists")
							 << false
							 << false
							 << QUrl("https://api.google.de/lists");

	QTest::newRow("newHostKeepStuff") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
									  << QUrl("//api.google.de/lists")
									  << true
									  << true
									  << QUrl("https://api.google.de/lists?p1=baum&p2=42#example");

	QTest::newRow("newUrl") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
							<< QUrl("ftp://api.google.de/lists")
							<< false
							<< false
							<< QUrl("ftp://api.google.de/lists");

	QTest::newRow("newUrlKeepStuff") << QUrl("https://user:password@api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42#example")
									 << QUrl("ftp://api.google.de/lists")
									 << true
									 << true
									 << QUrl("ftp://api.google.de/lists?p1=baum&p2=42#example");
}

void RequestBuilderTest::testBuildingRelative()
{
	QFETCH(QUrl, url);
	QFETCH(QUrl, relative);
	QFETCH(bool, mergeQuery);
	QFETCH(bool, keepFragment);
	QFETCH(QUrl, resultUrl);

	RequestBuilder builder(url);
	builder.updateFromRelativeUrl(relative, mergeQuery, keepFragment);
	QCOMPARE(builder.buildUrl(), resultUrl);
}

void RequestBuilderTest::testSending_data()
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

	map[QStringLiteral("title")] = QStringLiteral("baum");
	map.remove(QStringLiteral("body")); // workaround for QTBUG-80342
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

	QTest::newRow("testError.cbor") << server->url("/posts/baum")
									<< Testlib::CBody()
									<< QByteArray("GET")
									<< 404
									<< QNetworkReply::ContentNotFoundError
									<< Testlib::CBody();
	QTest::newRow("testError.json") << server->url("/posts/baum")
									<< Testlib::JBody()
									<< QByteArray("GET")
									<< 404
									<< QNetworkReply::ContentNotFoundError
									<< Testlib::JBody();
}

void RequestBuilderTest::testSending()
{
	QFETCH(QUrl, url);
	QFETCH(BodyType, body);
	QFETCH(QByteArray, verb);
	QFETCH(int, status);
	QFETCH(QNetworkReply::NetworkError, error);
	QFETCH(BodyType, object);

	RequestBuilder builder(url, nam);
	builder.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	if (!verb.isEmpty())
		builder.setVerb(verb);
	if (object.isValid()) {
		object.visit([&](const auto &vBody) {
			builder.setBody(vBody);
		});
	} else
		builder.setAccept(object.accept());

	auto reply = builder.send();
	QSignalSpy replySpy(reply, &QNetworkReply::finished);

	QVERIFY(replySpy.wait());
	QCOMPARE(reply->error(), error);
	QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), status);

	if(error == QNetworkReply::NoError) {
		auto repData = BodyType::parse(reply);
		QVERIFY(repData.isValid());
		QCOMPARE(repData, object);
	}

	reply->deleteLater();
}

void RequestBuilderTest::setPostParamsSending()
{
	QJsonObject repJson {
		{QStringLiteral("id"), 100},
		{QStringLiteral("username"), QStringLiteral("user")},
		{QStringLiteral("password"), QStringLiteral("super secret")}
	};

	RequestBuilder builder(server->url("/posts"), nam);
	builder.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);

	builder.setVerb("POST");
	builder.addPostParameter(QStringLiteral("username"), QStringLiteral("user"));
	builder.addPostParameter(QStringLiteral("password"), QStringLiteral("super secret"));

	auto reply = builder.send();
	QSignalSpy replySpy(reply, &QNetworkReply::finished);

	QVERIFY(replySpy.wait());
	QCOMPARE(reply->error(), QNetworkReply::NoError);
	QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

	QJsonParseError e;
	auto repData = QJsonDocument::fromJson(reply->readAll(), &e).object();
	QCOMPARE(e.error, QJsonParseError::NoError);
	QCOMPARE(repData, repJson);

	reply->deleteLater();
}

class TestThread : public QThread
{
public:
	TestThread() {
		setTerminationEnabled(true);
	}

	QUrl url;
	QNetworkAccessManager *nam;
	QJsonObject result;

protected:
	void run() override {
		RequestBuilder builder{url, nam};
		builder.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);

		builder.setVerb("POST");
		builder.addPostParameter(QStringLiteral("username"), QStringLiteral("user"));
		builder.addPostParameter(QStringLiteral("password"), QStringLiteral("super secret"));

		QFutureWatcher<QNetworkReply*> watcher;
		connect(&watcher, &QFutureWatcherBase::finished, [&](){
			auto sg = qScopeGuard([this](){
				exit(EXIT_FAILURE);
			});
			auto reply = watcher.result();
			QSignalSpy replySpy(reply, &QNetworkReply::finished);

			QCOMPARE(QThread::currentThread(), this);

			QVERIFY(replySpy.wait());
			QCOMPARE(reply->error(), QNetworkReply::NoError);
			QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

			QJsonParseError e;
			auto repData = QJsonDocument::fromJson(reply->readAll(), &e).object();
			QCOMPARE(e.error, QJsonParseError::NoError);
			QCOMPARE(repData, result);

			reply->deleteLater();
			sg.dismiss();
			quit();
		});
		watcher.setFuture(builder.sendAsync());
		QCOMPARE(exec(), EXIT_SUCCESS);
	}
};

void RequestBuilderTest::testAsyncSending()
{
	TestThread testThread;
	testThread.url = server->url("/posts");
	testThread.nam = nam;
	testThread.result = QJsonObject {
		{QStringLiteral("id"), 101},
		{QStringLiteral("username"), QStringLiteral("user")},
		{QStringLiteral("password"), QStringLiteral("super secret")}
	};

	testThread.start();
	auto sg = qScopeGuard([&](){
		testThread.terminate();
		QVERIFY(testThread.wait(1000));
	});
	QTRY_VERIFY(testThread.isFinished());
	sg.dismiss();
}

QTEST_MAIN(RequestBuilderTest)

#include "tst_requestbuilder.moc"
