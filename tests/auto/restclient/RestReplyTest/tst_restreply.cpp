#include "testlib.h"

#include <jphpost.h>
using namespace QtJsonSerializer;
using namespace QtRestClient;
using namespace std::chrono_literals;

class RestReplyTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testReplyWrapping_data();
	void testReplyWrapping();
	void testReplyError();
	void testReplyRetry();

	void testCallbackOverloads();

	void testGenericReplyWrapping_data();
	void testGenericReplyWrapping();

	void testGenericVoidReplyWrapping_data();
	void testGenericVoidReplyWrapping();

	void testGenericListReplyWrapping_data();
	void testGenericListReplyWrapping();

	void testGenericPagingReplyWrapping_data();
	void testGenericPagingReplyWrapping();
	void testPagingNext();
	void testPagingPrevious();
	void testPagingIterate();

	void testSimpleExtension();
	void testSimplePagingIterate();

private:
	HttpServer *server;
	RestClient *client;
	QNetworkAccessManager *nam;
};

void RestReplyTest::initTestCase()
{
	JsonSerializer::registerListConverters<JphPost*>();
	JsonSerializer::registerListConverters<JphPostSimple*>();
	server = new HttpServer(this);
	QVERIFY(server->setupRoutes());
	server->setAdvancedData();
	client = Testlib::createClient(this);
	client->setBaseUrl(server->url());
	qDebug() << client->baseUrl();
	nam = client->manager();
}

void RestReplyTest::cleanupTestCase()
{
	nam = nullptr;
	client->deleteLater();
	client = nullptr;
	server->deleteLater();
	server = nullptr;
}

void RestReplyTest::testReplyWrapping_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("status");
	QTest::addColumn<BodyType>("result");

	QCborMap map;
	map[QStringLiteral("id")] = 1;
	map[QStringLiteral("userId")] = 1;
	map[QStringLiteral("title")] = QStringLiteral("Title1");
	map[QStringLiteral("body")] = QStringLiteral("Body1");

	QTest::newRow("get.cbor") << server->url("/posts/1")
							  << true
							  << 200
							  << Testlib::CBody(map);
	QTest::newRow("get.json") << server->url("/posts/1")
							  << true
							  << 200
							  << Testlib::JBody(map);

	QTest::newRow("notFound.cbor") << server->url("/posts/435345")
								   << false
								   << 404
								   << Testlib::CBody();

	QTest::newRow("notFound.json") << server->url("/posts/435345")
								   << false
								   << 404
								   << Testlib::JBody();
}

void RestReplyTest::testReplyWrapping()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, status);
	QFETCH(BodyType, result);

	QNetworkRequest request(url);
	result.setAccept(request);
	bool called = false;

	auto reply = new RestReply(nam->get(request));
	reply->onSucceeded([&](int code, const RestReply::DataType &data){
		called = true;
		QVERIFY(succeed);
		QCOMPARE(code, status);
		QCOMPARE(BodyType{data}, result);
	});
	reply->onAllErrors([&](const QString &error, int code, QtRestClient::RestReply::Error type){
		called = true;
		QVERIFY2(!succeed, qUtf8Printable(error));
		QCOMPARE(type, QtRestClient::RestReply::Error::Failure);
		QCOMPARE(code, status);
	});

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QTRY_VERIFY(called);
	if (deleteSpy.isEmpty())
		QVERIFY(deleteSpy.wait());
	QCOMPARE(deleteSpy.size(), 1);
}

void RestReplyTest::testReplyError()
{
	QNetworkRequest request(server->url("/invalid"));
	request.setRawHeader("Accept", "application/cbor");

	bool called = false;

	auto reply = new QtRestClient::RestReply(nam->get(request));
	reply->onAllErrors([&](const QString &, int code, QtRestClient::RestReply::Error type){
		called = true;
		QCOMPARE(type, QtRestClient::RestReply::Error::Network);
		QCOMPARE(code, static_cast<int>(QNetworkReply::ContentNotFoundError));
	});
	QTRY_VERIFY(called);
}

void RestReplyTest::testReplyRetry()
{
	QNetworkRequest request(server->url("/invalid"));
	request.setRawHeader("Accept", "application/cbor");

	auto retryCount = 0ms;

	auto reply = new QtRestClient::RestReply(nam->get(request));
	reply->onAllErrors([&](const QString &, int code, QtRestClient::RestReply::Error type){
		retryCount++;
		QCOMPARE(type, QtRestClient::RestReply::Error::Network);
		QCOMPARE(code, static_cast<int>(QNetworkReply::ContentNotFoundError));
		if (retryCount < 3ms)
			reply->retryAfter((retryCount - 1ms) * 1500);//first 0, the 1500
	});

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(!deleteSpy.wait(1000));
	QVERIFY(deleteSpy.wait(14000));
	QCOMPARE(retryCount, 3ms);
}

void RestReplyTest::testCallbackOverloads()
{
	try {
		QNetworkRequest request(server->url("/posts/0"));

		const QCborMap testData {
			{QStringLiteral("id"), 0},
			{QStringLiteral("userId"), 0},
			{QStringLiteral("title"), QStringLiteral("Title0")},
			{QStringLiteral("body"), QStringLiteral("Body0")},
		};
		const QCborValue testVal {testData};

		client->setDataMode(RestClient::DataMode::Json);
		Testlib::setAccept(request, client);
		auto reply = new QtRestClient::RestReply{nam->get(request)};
		auto called = false;
		reply->onSucceeded([&](){
			called = true;
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code){
			called = true;
			QCOMPARE(code, 200);
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QJsonValue &value){
			called = true;
			QCOMPARE(value, testVal.toJsonValue());
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QJsonObject &value){
			called = true;
			QCOMPARE(value, testData.toJsonObject());
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QJsonArray &value){
			called = true;
			QCOMPARE(value, QJsonArray{});
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QJsonValue &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, testVal.toJsonValue());
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QJsonObject &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, testData.toJsonObject());
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QJsonArray &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, QJsonArray{});
		});
		QTRY_VERIFY(called);

		client->setDataMode(RestClient::DataMode::Cbor);
		Testlib::setAccept(request, client);
		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QCborValue &value){
			called = true;
			QCOMPARE(value, testVal);
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QCborMap &value){
			called = true;
			QCOMPARE(value, testData);
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](const QCborArray &value){
			called = true;
			QCOMPARE(value, QCborArray{});
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QCborValue &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, testVal);
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QCborMap &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, testData);
		});
		QTRY_VERIFY(called);

		reply = new QtRestClient::RestReply{nam->get(request)};
		called = false;
		reply->onSucceeded([&](int code, const QCborArray &value){
			called = true;
			QCOMPARE(code, 200);
			QCOMPARE(value, QCborArray{});
		});
		QTRY_VERIFY(called);
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testGenericReplyWrapping_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("status");
	QTest::addColumn<QObject*>("result");
	QTest::addColumn<bool>("except");

	QTest::newRow("get") << server->url("/posts/1")
						 << true
						 << 200
						 << static_cast<QObject*>(JphPost::createDefault(this))
						 << false;

	QTest::newRow("notFound") << server->url("/posts/834")
							  << false
							  << 404
							  << new QObject(this)
							  << false;

	QTest::newRow("serExcept") << server->url("/posts")
							   << false
							   << 0
							   << new QObject(this)
							   << true;
}

void RestReplyTest::testGenericReplyWrapping()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, status);
	QFETCH(QObject*, result);
	QFETCH(bool, except);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(url);
			Testlib::setAccept(request, client);

			bool called = false;

			auto reply = new QtRestClient::GenericRestReply<JphPost*, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code, JphPost *data){
				called = true;
				QVERIFY(succeed);
				QVERIFY(!except);
				QCOMPARE(code, status);
				QVERIFY(JphPost::equals(data, result));
				data->deleteLater();
			});
			reply->onAllErrors([&](const QString &error, int code, QtRestClient::RestReply::Error type){
				called = true;
				QVERIFY2(!succeed, qUtf8Printable(error));
				if (except)
					QCOMPARE(type, QtRestClient::RestReply::Error::Deserialization);
				else {
					QCOMPARE(type, QtRestClient::RestReply::Error::Failure);
					QCOMPARE(code, status);
				}
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
	result->deleteLater();
}

void RestReplyTest::testGenericVoidReplyWrapping_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("status");
	QTest::addColumn<bool>("except");

	QTest::newRow("get.value") << server->url("/posts/1")
							   << true
							   << 200
							   << false;
	QTest::newRow("get.empty.ok") << server->url("/void/false")
								  << true
								  << 200
								  << false;
	QTest::newRow("get.empty.noContent") << server->url("/void/true")
										 << true
										 << 204
										 << false;

	QTest::newRow("notFound") << server->url("/posts/3434")
							  << false
							  << 404
							  << false;
}

void RestReplyTest::testGenericVoidReplyWrapping()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, status);
	QFETCH(bool, except);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(url);
			Testlib::setAccept(request, client);

			bool called = false;

			auto reply = new QtRestClient::GenericRestReply<void, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code){
				called = true;
				QVERIFY(succeed);
				QVERIFY(!except);
				QCOMPARE(code, status);
			});
			reply->onAllErrors([&](const QString &error, int code, QtRestClient::RestReply::Error type){
				called = true;
				QVERIFY2(!succeed, qUtf8Printable(error));
				if(except)
					QCOMPARE(type, QtRestClient::RestReply::Error::Deserialization);
				else {
					QCOMPARE(type, QtRestClient::RestReply::Error::Failure);
					QCOMPARE(code, status);
				}
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testGenericListReplyWrapping_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("status");
	QTest::addColumn<int>("count");
	QTest::addColumn<QObject*>("firstResult");
	QTest::addColumn<bool>("except");

	QTest::newRow("get") << server->url("/posts")
						 << true
						 << 200
						 << 100
						 << static_cast<QObject*>(JphPost::createFirst(this))
						 << false;

	QTest::newRow("notFound") << server->url("/posts/3422")
							  << false
							  << 404
							  << 0
							  << new QObject(this)
							  << false;

	QTest::newRow("serExcept") << server->url("/posts/1")
							   << false
							   << 0
							   << 0
							   << new QObject(this)
							   << true;
}

void RestReplyTest::testGenericListReplyWrapping()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, status);
	QFETCH(int, count);
	QFETCH(QObject*, firstResult);
	QFETCH(bool, except);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(url);
			Testlib::setAccept(request, client);

			bool called = false;

			auto reply = new QtRestClient::GenericRestReply<QList<JphPost*>, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code, QList<JphPost*> data){
				called = true;
				QVERIFY(succeed);
				QVERIFY(!except);
				QCOMPARE(code, status);
				QCOMPARE(data.size(), count);
				QVERIFY(JphPost::equals(data.first(), firstResult));
				qDeleteAll(data);
			});
			reply->onAllErrors([&](const QString &error, int code, QtRestClient::RestReply::Error type){
				called = true;
				QVERIFY2(!succeed, qUtf8Printable(error));
				if(except)
					QCOMPARE(type, QtRestClient::RestReply::Error::Deserialization);
				else {
					QCOMPARE(type, QtRestClient::RestReply::Error::Failure);
					QCOMPARE(code, status);
				}
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
	firstResult->deleteLater();
}

void RestReplyTest::testGenericPagingReplyWrapping_data()
{
	QTest::addColumn<QUrl>("url");
	QTest::addColumn<bool>("succeed");
	QTest::addColumn<int>("status");
	QTest::addColumn<int>("offset");
	QTest::addColumn<int>("total");
	QTest::addColumn<QObject*>("firstResult");
	QTest::addColumn<bool>("except");

	QTest::newRow("get") << server->url("/pages/0")
						 << true
						 << 200
						 << 0
						 << 100
						 << static_cast<QObject*>(JphPost::createFirst(this))
						 << false;

	QTest::newRow("notFound") << server->url("/pages/3422")
							  << false
							  << 404
							  << 0
							  << 0
							  << new QObject(this)
							  << false;
}

void RestReplyTest::testGenericPagingReplyWrapping()
{
	QFETCH(QUrl, url);
	QFETCH(bool, succeed);
	QFETCH(int, status);
	QFETCH(int, offset);
	QFETCH(int, total);
	QFETCH(QObject*, firstResult);
	QFETCH(bool, except);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(url);
			Testlib::setAccept(request, client);

			bool called = false;

			auto reply = new QtRestClient::GenericRestReply<QtRestClient::Paging<JphPost*>, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code, const QtRestClient::Paging<JphPost*> &data){
				called = true;
				QVERIFY(succeed);
				QVERIFY(!except);
				QCOMPARE(code, status);
				QVERIFY(data.isValid());
				QCOMPARE(data.offset(), offset);
				QCOMPARE(data.total(), total);
				QVERIFY(JphPost::equals(data.items().first(), firstResult));
				data.deleteAllItems();
			});
			reply->onAllErrors([&](const QString &error, int code, QtRestClient::RestReply::Error type){
				called = true;
				QVERIFY2(!succeed, qUtf8Printable(error));
				if(except)
					QCOMPARE(type, QtRestClient::RestReply::Error::Deserialization);
				else {
					QCOMPARE(type, QtRestClient::RestReply::Error::Failure);
					QCOMPARE(code, status);
				}
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
	firstResult->deleteLater();
}

void RestReplyTest::testPagingNext()
{
	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(server->url("/pages/0"));
			Testlib::setAccept(request, client);

			bool called = false;
			QtRestClient::Paging<JphPost*> firstPaging;

			auto reply = new QtRestClient::GenericRestReply<QtRestClient::Paging<JphPost*>, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code, const QtRestClient::Paging<JphPost*> &data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(data.isValid());
				QVERIFY(data.properties().contains("id"));
				QCOMPARE(data.properties()["id"].toInt(), 0);
				QVERIFY(!data.hasPrevious());
				QVERIFY(data.hasNext());
				QCOMPARE(data.offset(), 0);
				QCOMPARE(data.total(), 100);
				data.deleteAllItems();
				firstPaging = data;
			});
			reply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);

			QVERIFY(firstPaging.isValid());
			called = false;
			auto nextReply = firstPaging.next<QString>();
			nextReply->onSucceeded([&](int code, const QtRestClient::Paging<JphPost*> &data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(data.isValid());
				QVERIFY(data.properties().contains("id"));
				QCOMPARE(data.properties()["id"].toInt(), 1);
				QCOMPARE(data.offset(), 10);
				QCOMPARE(data.total(), 100);
				data.deleteAllItems();
			});
			nextReply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testPagingPrevious()
{
	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(server->url("/pages/9"));
			Testlib::setAccept(request, client);

			bool called = false;
			QtRestClient::Paging<JphPost*> lastPaging;

			auto reply = new QtRestClient::GenericRestReply<QtRestClient::Paging<JphPost*>, QString>(nam->get(request), client);
			reply->onSucceeded([&](int code, const QtRestClient::Paging<JphPost*> &data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(data.isValid());
				QVERIFY(data.properties().contains("id"));
				QCOMPARE(data.properties()["id"].toInt(), 9);
				QVERIFY(data.hasPrevious());
				QVERIFY(!data.hasNext());
				QCOMPARE(data.offset(), 90);
				QCOMPARE(data.total(), 100);
				data.deleteAllItems();
				lastPaging = data;
			});
			reply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);

			QVERIFY(lastPaging.isValid());
			called = false;
			auto prevReply = lastPaging.previous<QString>();
			prevReply->onSucceeded([&](int code, const QtRestClient::Paging<JphPost*> &data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(data.isValid());
				QVERIFY(data.properties().contains("id"));
				QCOMPARE(data.properties()["id"].toInt(), 8);
				QCOMPARE(data.offset(), 80);
				QCOMPARE(data.total(), 100);
				data.deleteAllItems();
			});
			prevReply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testPagingIterate()
{
	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(server->url("/pages/0"));
			Testlib::setAccept(request, client);

			auto count = 0;
			auto reply = new QtRestClient::GenericRestReply<QtRestClient::Paging<JphPost*>, QString>(nam->get(request), client);
			reply->iterate([&](JphPost *data, int index){
				auto ok = false;
				[&](){
					QVERIFY(data);
					QCOMPARE(index, count);
					QCOMPARE(data->id, count++);
					ok = true;
				}();
				if (!ok)
					count = 101;
				data->deleteLater();
				return ok;
			});
			reply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				count = 101;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_COMPARE(count, 100);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testSimpleExtension()
{
	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			auto simple = new JphPostSimple(1, "Title1", QStringLiteral("/posts/1"), this);
			auto full = JphPost::createDefault(this);

			QVERIFY(simple->hasExtension());
			QCOMPARE(simple->extensionHref(), simple->href);
			QVERIFY(!simple->currentExtended());

			bool called = false;
			// extend first try
			simple->extend<QString>(client, [&](JphPost *data, bool networkLoaded){
				called = true;
				QVERIFY(networkLoaded);
				QVERIFY(full->equals(data));
			}, [&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);
			QVERIFY(full->equals(simple->currentExtended()));

			// test already loaded extension
			called = false;
			simple->extend<QString>(client, [&](JphPost *data, bool networkLoaded){
				called = true;
				QVERIFY(!networkLoaded);
				QVERIFY(full->equals(data));
			}, [&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QVERIFY(called);

			delete simple->currentExtended();
			QVERIFY(!simple->currentExtended());

			// network load
			called = false;
			auto reply = simple->extend<QString>(client);
			reply->onSucceeded([&](int code, JphPost *data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(full->equals(data));
				data->deleteLater();
			});
			reply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_VERIFY(called);
			QVERIFY(full->equals(simple->currentExtended()));

			simple->deleteLater();
			full->deleteLater();
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

void RestReplyTest::testSimplePagingIterate()
{
	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);
			QNetworkRequest request(server->url("/pagelets/0"));
			Testlib::setAccept(request, client);

			auto count = 0;
			auto reply = new GenericRestReply<Paging<JphPostSimple*>, QString>(nam->get(request), client);
			reply->iterate([&](JphPostSimple *data, int index){
				auto ok = false;
				[&](){
					QVERIFY(data);
					QCOMPARE(index, -1);
					QCOMPARE(data->id, count++);
					ok = true;
				}();
				if (!ok)
					count = 101;
				data->deleteLater();
				return ok;
			});
			reply->onAllErrors([&](const QString &error, int, QtRestClient::RestReply::Error){
				count = 101;
				QFAIL(qUtf8Printable(error));
			});
			QTRY_COMPARE(count, 100);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}
}

QTEST_MAIN(RestReplyTest)

#include "tst_restreply.moc"
