#include <QtCore>
#include <QtTest>
#include <QtRestClient>
#include <qtcoroutine.h>
#include <testlib.h>
using namespace QtRestClient;
using namespace QtCoroutine;

class RestAwaitablesTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testRestReplyAwait_data();
	void testRestReplyAwait();

	void testGenericRestReplyAwait_data();
	void testGenericRestReplyAwait();
	void testGenericVoidRestReplyAwait_data();
	void testGenericVoidRestReplyAwait();

private:
	HttpServer *server = nullptr;
	RestClient *client = nullptr;
	QNetworkAccessManager *nam = nullptr;
};

void RestAwaitablesTest::initTestCase()
{
	server = new HttpServer{this};
	QVERIFY(server->setupRoutes());
	server->setDefaultData();
	client = Testlib::createClient(this);
	client->setBaseUrl(server->url());
	nam = client->manager();
}

void RestAwaitablesTest::cleanupTestCase()
{
	nam = nullptr;
	client->deleteLater();
	client = nullptr;
	server->deleteLater();
	server = nullptr;
}

void RestAwaitablesTest::testRestReplyAwait_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<BodyType>("result");
	QTest::addColumn<int>("code");
	QTest::addColumn<RestReply::Error>("type");

	QCborMap data {
		{QStringLiteral("id"), 1},
		{QStringLiteral("userId"), 1},
		{QStringLiteral("title"), QStringLiteral("Title1")},
		{QStringLiteral("body"), QStringLiteral("Body1")}
	};

	QTest::newRow("get.cbor") << server->url("/posts/1")
							  << true
							  << Testlib::CBody(data)
							  << 200
							  << RestReply::Error::Network;
	QTest::newRow("get.data") << server->url("/posts/1")
							  << true
							  << Testlib::JBody(data)
							  << 200
							  << RestReply::Error::Network;

	QTest::newRow("notFound.cbor") << server->url("/posts/2334")
								   << false
								   << Testlib::CBody()
								   << 404
								   << RestReply::Error::Failure;
	QTest::newRow("notFound.json") << server->url("/posts/2334")
								   << false
								   << Testlib::JBody()
								   << 404
								   << RestReply::Error::Failure;

	QTest::newRow("invalid.cbor") << server->url("/invalid")
								  << false
								  << Testlib::CBody()
								  << static_cast<int>(QNetworkReply::ContentNotFoundError)
								  << RestReply::Error::Network;
	QTest::newRow("invalid.json") << server->url("/invalid")
								  << false
								  << Testlib::JBody()
								  << static_cast<int>(QNetworkReply::ContentNotFoundError)
								  << RestReply::Error::Network;
}

void RestAwaitablesTest::testRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(BodyType, result);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	QEventLoop loop;
	auto res = createAndRun([&](){
		QNetworkRequest request{url};
		result.setAccept(request);
		auto reply = new RestReply{nam->get(request), this};
		try {
			bool ok = false;
			[&](){
				if (succeed) {
					auto data = await(reply->awaitable());
					QCOMPARE(BodyType{data}, result);
				} else {
					try {
						await(reply->awaitable());
						QFAIL("Await successed. Expected AwaitedException");
					} catch (AwaitedException &e) {
						QCOMPARE(e.errorType(), type);
						QCOMPARE(e.errorCode(), code);
						if (result.isValid())
							QCOMPARE(e.errorData(), result.toVariant());
					}
				}
				ok = true;
			}();
			loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
		} catch (std::exception &e) {
			loop.exit(EXIT_FAILURE);
			QFAIL(e.what());
		}
	});
	QVERIFY(res.first != InvalidRoutineId);
	QCOMPARE(res.second, Paused);
	QCOMPARE(loop.exec(), EXIT_SUCCESS);
}

void RestAwaitablesTest::testGenericRestReplyAwait_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<JphPost*>("result");
	QTest::addColumn<int>("code");
	QTest::addColumn<RestReply::Error>("type");

	QTest::newRow("get") << server->url("/posts/1")
						 << true
						 << JphPost::createDefault(this)
						 << 200
						 << RestReply::Error::Network;

	QTest::newRow("notFound") << server->url("/posts/34234")
							  << false
							  << static_cast<JphPost*>(nullptr)
							  << 404
							  << RestReply::Error::Failure;

	QTest::newRow("invalid") << server->url("/invalid")
							 << false
							 << static_cast<JphPost*>(nullptr)
							 << static_cast<int>(QNetworkReply::ContentNotFoundError)
							 << RestReply::Error::Network;
}

void RestAwaitablesTest::testGenericRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(JphPost*, result);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
		client->setDataMode(mode);
		QEventLoop loop;
		auto res = createAndRun([&](){
			QNetworkRequest request{url};
			Testlib::setAccept(request, client);
			auto reply = new GenericRestReply<JphPost*, QString>{nam->get(request), client, this};
			try {
				bool ok = false;
				[&](){
					if (succeed) {
						auto data = await(reply->awaitable());
						QVERIFY(JphPost::equals(data, result));
					} else {
						try {
							await(reply->awaitable());
							QFAIL("Await successed. Expected AwaitedException");
						} catch (GenericAwaitedException<QString> &e) {
							QCOMPARE(e.errorType(), type);
							QCOMPARE(e.errorCode(), code);
						}
					}
					ok = true;
				}();
				loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
			} catch (std::exception &e) {
				loop.exit(EXIT_FAILURE);
				QFAIL(e.what());
			}
		});
		QVERIFY(res.first != InvalidRoutineId);
		QCOMPARE(res.second, Paused);
		QCOMPARE(loop.exec(), EXIT_SUCCESS);
	}
}

void RestAwaitablesTest::testGenericVoidRestReplyAwait_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("code");
	QTest::addColumn<RestReply::Error>("type");

	QTest::newRow("get") << server->url("/posts/1")
						 << true
						 << 200
						 << RestReply::Error::Network;

	QTest::newRow("notFound") << server->url("/posts/34234")
							  << false
							  << 404
							  << RestReply::Error::Failure;

	QTest::newRow("invalid") << server->url("/invalid")
							 << false
							 << 203
							 << RestReply::Error::Network;
}

void RestAwaitablesTest::testGenericVoidRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
		client->setDataMode(mode);
		QEventLoop loop;
		auto res = createAndRun([&](){
			QNetworkRequest request{url};
			Testlib::setAccept(request, client);
			auto reply = new GenericRestReply<void, QString>{nam->get(request), client, this};
			try {
				bool ok = false;
				[&](){
					if (succeed)
						await(reply->awaitable());
					else {
						try {
							await(reply->awaitable());
							QFAIL("Await successed. Expected AwaitedException");
						} catch (GenericAwaitedException<QString> &e) {
							QCOMPARE(e.errorType(), type);
							QCOMPARE(e.errorCode(), code);
						}
					}
					ok = true;
				}();
				loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
			} catch (std::exception &e) {
				loop.exit(EXIT_FAILURE);
				QFAIL(e.what());
			}
		});
		QVERIFY(res.first != InvalidRoutineId);
		QCOMPARE(res.second, Paused);
		QCOMPARE(loop.exec(), EXIT_SUCCESS);
	}
}

QTEST_MAIN(RestAwaitablesTest)

#include "tst_restawaitables.moc"
