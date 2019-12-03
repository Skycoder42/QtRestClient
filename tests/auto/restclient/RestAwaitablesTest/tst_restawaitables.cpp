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
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<int>("code");
	QTest::addColumn<RestReply::Error>("type");

	QTest::newRow("get") << server->url("/posts/1")
						 << true
						 << QJsonObject {
									{QStringLiteral("userId"), 1},
									{QStringLiteral("id"), 1},
									{QStringLiteral("title"), QStringLiteral("Title1")},
									{QStringLiteral("body"), QStringLiteral("Body1")},
								}
						 << 200
						 << RestReply::Network;

	QTest::newRow("notFound") << server->url("/posts/baum")
							  << false
							  << QJsonObject{}
							  << 404
							  << RestReply::Failure;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << QJsonObject()
							 << 203
							 << RestReply::Network;
}

void RestAwaitablesTest::testRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(QJsonObject, result);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	QEventLoop loop;
	auto res = createAndRun([&](){
		auto reply = new RestReply{nam->get(QNetworkRequest{url}), this};
		try {
			bool ok = false;
			[&](){
				if(succeed) {
					auto data = std::get<QJsonValue>(await(reply->awaitable()));  // TODO clean
					QVERIFY(data.isObject());
					QCOMPARE(data.toObject(), result);
				} else {
					try {
						await(reply->awaitable());
						QFAIL("Await successed. Expected AwaitedException");
					} catch (AwaitedException &e) {
						QCOMPARE(e.errorType(), type);
						QCOMPARE(e.errorCode(), code);
						if(!result.isEmpty())
							QCOMPARE(e.errorObject(), result);
					}
				}
				ok = true;
			}();
			loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
		} catch(std::exception &e) {
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
						 << RestReply::Network;

	QTest::newRow("notFound") << server->url("/posts/34234")
							  << false
							  << static_cast<JphPost*>(nullptr)
							  << 404
							  << RestReply::Failure;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << static_cast<JphPost*>(nullptr)
							 << 203
							 << RestReply::Network;
}

void RestAwaitablesTest::testGenericRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(JphPost*, result);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	QEventLoop loop;
	auto res = createAndRun([&](){
		auto reply = new GenericRestReply<JphPost*>{nam->get(QNetworkRequest{url}), client, this};
		try {
			bool ok = false;
			[&](){
				if(succeed) {
					auto data = await(reply->awaitable());
					QVERIFY(JphPost::equals(data, result));
				} else {
					try {
						await(reply->awaitable());
						QFAIL("Await successed. Expected AwaitedException");
					} catch (GenericAwaitedException<QObject*> &e) {
						QCOMPARE(e.errorType(), type);
						QCOMPARE(e.errorCode(), code);
					}
				}
				ok = true;
			}();
			loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
		} catch(std::exception &e) {
			loop.exit(EXIT_FAILURE);
			QFAIL(e.what());
		}
	});
	QVERIFY(res.first != InvalidRoutineId);
	QCOMPARE(res.second, Paused);
	QCOMPARE(loop.exec(), EXIT_SUCCESS);
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
						 << RestReply::Network;

	QTest::newRow("notFound") << server->url("/posts/34234")
							  << false
							  << 404
							  << RestReply::Failure;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << 203
							 << RestReply::Network;
}

void RestAwaitablesTest::testGenericVoidRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, code);
	QFETCH(RestReply::Error, type);

	QEventLoop loop;
	auto res = createAndRun([&](){
		auto reply = new GenericRestReply<void>{nam->get(QNetworkRequest{url}), client, this};
		try {
			bool ok = false;
			[&](){
				if(succeed) {
					await(reply->awaitable());
				} else {
					try {
						await(reply->awaitable());
						QFAIL("Await successed. Expected AwaitedException");
					} catch (GenericAwaitedException<QObject*> &e) {
						QCOMPARE(e.errorType(), type);
						QCOMPARE(e.errorCode(), code);
					}
				}
				ok = true;
			}();
			loop.exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
		} catch(std::exception &e) {
			loop.exit(EXIT_FAILURE);
			QFAIL(e.what());
		}
	});
	QVERIFY(res.first != InvalidRoutineId);
	QCOMPARE(res.second, Paused);
	QCOMPARE(loop.exec(), EXIT_SUCCESS);
}

QTEST_MAIN(RestAwaitablesTest)

#include "tst_restawaitables.moc"
