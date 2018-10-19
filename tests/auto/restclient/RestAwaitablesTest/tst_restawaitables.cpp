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
	server->verifyRunning();
	server->setDefaultData();
	client = Testlib::createClient(this);
	client->setBaseUrl(QStringLiteral("http://localhost:%1").arg(server->serverPort()));
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
	QTest::addColumn<RestReply::ErrorType>("type");

	QTest::newRow("get") << server->url("posts/1")
						 << true
						 << QJsonObject {
									{QStringLiteral("userId"), 1},
									{QStringLiteral("id"), 1},
									{QStringLiteral("title"), QStringLiteral("Title1")},
									{QStringLiteral("body"), QStringLiteral("Body1")},
								}
						 << 200
						 << RestReply::NetworkError;

	QTest::newRow("notFound") << server->url("posts/baum")
							  << false
							  << QJsonObject{{QStringLiteral("message"), QStringLiteral("path not found")}}
							  << 404
							  << RestReply::FailureError;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << QJsonObject()
							 << 203
							 << RestReply::NetworkError;
}

void RestAwaitablesTest::testRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(QJsonObject, result);
	QFETCH(int, code);
	QFETCH(RestReply::ErrorType, type);

	QEventLoop loop;
	auto res = createAndRun([&](){
		auto reply = new RestReply{nam->get(QNetworkRequest{url}), this};
		try {
			bool ok = false;
			[&](){
				if(succeed) {
					auto data = await(reply->awaitable());
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
	QTest::addColumn<RestReply::ErrorType>("type");

	QTest::newRow("get") << server->url("posts/1")
						 << true
						 << JphPost::createDefault(this)
						 << 200
						 << RestReply::NetworkError;

	QTest::newRow("notFound") << server->url("posts/baum")
							  << false
							  << static_cast<JphPost*>(nullptr)
							  << 404
							  << RestReply::FailureError;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << static_cast<JphPost*>(nullptr)
							 << 203
							 << RestReply::NetworkError;
}

void RestAwaitablesTest::testGenericRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(JphPost*, result);
	QFETCH(int, code);
	QFETCH(RestReply::ErrorType, type);

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
						if(type == RestReply::FailureError) {
							QVERIFY(e.genericError());
							QVERIFY(e.genericError()->dynamicPropertyNames().contains("message"));
						}
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
	QTest::addColumn<RestReply::ErrorType>("type");

	QTest::newRow("get") << server->url("posts/1")
						 << true
						 << 200
						 << RestReply::NetworkError;

	QTest::newRow("notFound") << server->url("posts/baum")
							  << false
							  << 404
							  << RestReply::FailureError;

	QTest::newRow("invalid") << QUrl{QStringLiteral("http://example.com/non/existant/api")}
							 << false
							 << 203
							 << RestReply::NetworkError;
}

void RestAwaitablesTest::testGenericVoidRestReplyAwait()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, code);
	QFETCH(RestReply::ErrorType, type);

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
						if(type == RestReply::FailureError) {
							QVERIFY(e.genericError());
							QVERIFY(e.genericError()->dynamicPropertyNames().contains("message"));
						}
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
