#include "jphuser.h"
#include "testlib.h"
using namespace QtRestClient;
using namespace QtJsonSerializer;

class IntegrationTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testRawChain();
	void testQObjectChain();
	void testQObjectListChain();
	void testQObjectPagingChain();

	void testAsync();
	void testAsyncPaging();

private:
	HttpServer *server;
	RestClient *client;
};

void IntegrationTest::initTestCase()
{
	JsonSerializer::registerListConverters<JphPost*>();
	server = new HttpServer(this);
	QVERIFY(server->setupRoutes());
	server->setAdvancedData();
	client = Testlib::createClient(this);
	client->setBaseUrl(server->url());
}

void IntegrationTest::cleanupTestCase()
{
	client->deleteLater();
	client = nullptr;
}

void IntegrationTest::testRawChain()
{
	QCborMap map;
	map[QStringLiteral("id")] = 1;
	map[QStringLiteral("userId")] = 42;
	map[QStringLiteral("title")] = QStringLiteral("baum");
	map[QStringLiteral("body")] = QStringLiteral("baum");

	auto postClass = client->createClass("posts", client);

	auto called = false;
	client->setDataMode(RestClient::DataMode::Cbor);
	auto reply = postClass->callRaw(RestClass::PutVerb, "1", map);
	reply->onSucceeded([&](int code, const QCborMap &data){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(data, map);
	});
	reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
		called = true;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});
	QTRY_VERIFY(called);

	called = false;
	client->setDataMode(RestClient::DataMode::Json);
	reply = postClass->callRaw(RestClass::PutVerb, "1", map.toJsonObject());
	reply->onSucceeded([&](int code, const QJsonObject &data){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(data, map.toJsonObject());
	});
	reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
		called = true;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});
	QTRY_VERIFY(called);

	postClass->deleteLater();
}

void IntegrationTest::testQObjectChain()
{
	auto object = new JphPost(2, 42, "baum", "baum", this);
	auto postClass = client->createClass("posts", client);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);

			auto called = false;
			auto reply = postClass->put<JphPost*, QString>("2", object);
			reply->onSucceeded([&](int code, JphPost *data){
				called = true;
				QCOMPARE(code, 200);
				QVERIFY(JphPost::equals(data, object));
				data->deleteLater();
			});
			reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}

	postClass->deleteLater();
	object->deleteLater();
}

void IntegrationTest::testQObjectListChain()
{
	auto postClass = client->createClass("posts", client);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);

			bool called = false;
			auto reply = postClass->get<QList<JphPost*>, QString>();
			reply->onSucceeded([&](int code, const QList<JphPost*> &data){
				called = true;
				QCOMPARE(code, 200);
				QCOMPARE(data.size(), 100);
				qDeleteAll(data);
			});
			reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
				called = true;
				QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
			});
			QTRY_VERIFY(called);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}

	postClass->deleteLater();
}

void IntegrationTest::testQObjectPagingChain()
{
	auto pagingClass = client->createClass("pages", client);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);

			auto count = 0;
			auto reply = pagingClass->get<Paging<JphPost*>, QString>("0");
			reply->iterate([&](JphPost* data, int index){
				auto ok = false;
				[&](){
					QVERIFY(data);
					QCOMPARE(index, count);
					QCOMPARE(data->id, count++);
					ok = true;
				}();
				data->deleteLater();
				return ok;
			});
			reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
				count = 101;
				QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
			});
			QTRY_COMPARE(count, 100);
		}
	} catch (std::exception &e) {
		QFAIL(e.what());
	}

	pagingClass->deleteLater();
}

void IntegrationTest::testAsync()
{
	client->setThreaded(true);
	client->setAsyncPool(QThreadPool::globalInstance());

	const auto cThread = QThread::currentThread();
	auto obj1 = JphPost::create(5, this);
	auto obj2 = JphPost::create(50, this);

	auto postClass = client->createClass("posts", client);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);

			auto called = false;
			postClass->get<JphPost*, QString>("5")
				->onSucceeded([&](int code1, JphPost *data1){
					auto sg = qScopeGuard([&](){
						called = true;
					});
					QVERIFY(QThread::currentThread() != cThread);
					QCOMPARE(code1, 200);
					QVERIFY(JphPost::equals(data1, obj1));
					data1->deleteLater();

					postClass->get<JphPost*, QString>("50")
						->onSucceeded([&](int code2, JphPost *data2){
							called = true;
							QVERIFY(QThread::currentThread() != cThread);
							QCOMPARE(code2, 200);
							QVERIFY(JphPost::equals(data2, obj2));
							data2->deleteLater();
						})->onAllErrors([&](const QString &error, int code, RestReply::Error){
							called = true;
							QVERIFY(QThread::currentThread() != cThread);
							QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
						});

					sg.dismiss();
				})->onAllErrors([&](const QString &error, int code, RestReply::Error){
					called = true;
					QVERIFY(QThread::currentThread() != cThread);
					QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
				});
			QTRY_VERIFY(called);
		}
		QVERIFY(client->asyncPool()->waitForDone());
	} catch (std::exception &e) {
		client->asyncPool()->clear();
		client->asyncPool()->waitForDone();
		QFAIL(e.what());
	}

	obj1->deleteLater();
	obj2->deleteLater();
}

void IntegrationTest::testAsyncPaging()
{
	client->setThreaded(true);
	client->setAsyncPool(QThreadPool::globalInstance());

	const auto cThread = QThread::currentThread();
	auto pagingClass = client->createClass("pages", client);

	try {
		for (auto mode : {RestClient::DataMode::Cbor, RestClient::DataMode::Json}) {
			client->setDataMode(mode);

			auto count = 0;
			auto reply = pagingClass->get<Paging<JphPost*>, QString>("0");
			reply->iterate([&](JphPost* data, int index){
				auto ok = false;
				[&](){
					QVERIFY(QThread::currentThread() != cThread);
					QVERIFY(data);
					QCOMPARE(index, count);
					QCOMPARE(data->id, count++);
					ok = true;
				}();
				data->deleteLater();
				return ok;
			});
			reply->onAllErrors([&](const QString &error, int code, RestReply::Error){
				count = 101;
				QVERIFY(QThread::currentThread() != cThread);
				QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
			});
			QTRY_COMPARE(count, 100);
		}
		QVERIFY(client->asyncPool()->waitForDone());
	} catch (std::exception &e) {
		client->asyncPool()->clear();
		client->asyncPool()->waitForDone();
		QFAIL(e.what());
	}

	pagingClass->deleteLater();
}

template <typename TRest, typename TErr = QObject*>
static void DO_NOT_CALL_compilation_test_reply()
{
	GenericRestReply<TRest, TErr> *rep = nullptr;
	rep->onSucceeded({});
	rep->onSucceeded(nullptr, {});
	rep->onFailed({});
	rep->onFailed(nullptr, {});
	rep->onSerializeException({});
	rep->onAllErrors({}, {});
	rep->onAllErrors(nullptr, {}, {});
	rep->onCompleted({});
	rep->onCompleted(nullptr, {});
	rep->onError({});
	rep->onError(nullptr, {});
	rep->disableAutoDelete();
}

template <typename T>
static void DO_NOT_CALL_compilation_test_paging()
{
	Paging<T> paging;
	paging.iterate({}, 10, 5);
	paging.iterate(nullptr, {}, 10, 5);
	paging.iterate({}, {}, {}, 10, 5);
	paging.iterate(nullptr, {}, {}, {}, 10, 5);
	paging.iterate({}, {}, {}, {}, 10, 5);
	paging.iterate(nullptr, {}, {}, {}, {}, 10, 5);
	GenericRestReply<Paging<T>> *rep = nullptr;
	rep->iterate({}, 10, 5);
	rep->iterate(nullptr, {}, 10, 5);
}

template <typename T>
static void DO_NOT_CALL_compilation_test_class()
{
	T object;
	QList<T> list;
	RestClass *postClass = nullptr;

	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"));
	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"), object);
	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"), list);
	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb);
	postClass->call<T>(RestClass::GetVerb, object);
	postClass->call<T>(RestClass::GetVerb, list);
	postClass->call<T>(RestClass::GetVerb, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, object, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, list, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, QUrl("test"));
	postClass->call<T>(RestClass::GetVerb, QUrl("test"), object);
	postClass->call<T>(RestClass::GetVerb, QUrl("test"), list);
	postClass->call<T>(RestClass::GetVerb, QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->call<T>(RestClass::GetVerb, QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->get<T>(QStringLiteral("test"));
	postClass->get<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->get<T>();
	postClass->get<T>(RestClass::concatParams("baum", 42));
	postClass->get<T>(QUrl("test"));
	postClass->get<T>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->post<T>(QStringLiteral("test"));
	postClass->post<T>(QStringLiteral("test"), object);
	postClass->post<T>(QStringLiteral("test"), list);
	postClass->post<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->post<T>();
	postClass->post<T>(object);
	postClass->post<T>(list);
	postClass->post<T>(RestClass::concatParams("baum", 42));
	postClass->post<T>(QUrl("test"));
	postClass->post<T>(QUrl("test"), object);
	postClass->post<T>(QUrl("test"), list);
	postClass->post<T>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->put<T>(QStringLiteral("test"), object);
	postClass->put<T>(QStringLiteral("test"), list);
	postClass->put<T>(object);
	postClass->put<T>(list);
	postClass->put<T>(QUrl("test"), object);
	postClass->put<T>(QUrl("test"), list);

	postClass->deleteResource<T>(QStringLiteral("test"));
	postClass->deleteResource<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->deleteResource<T>();
	postClass->deleteResource<T>(RestClass::concatParams("baum", 42));
	postClass->deleteResource<T>(QUrl("test"));
	postClass->deleteResource<T>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->patch<T>(QStringLiteral("test"), object);
	postClass->patch<T>(QStringLiteral("test"), list);
	postClass->patch<T>(object);
	postClass->patch<T>(list);
	postClass->patch<T>(QUrl("test"), object);
	postClass->patch<T>(QUrl("test"), list);
}

static void DO_NOT_CALL_compilation_test()
{
	DO_NOT_CALL_compilation_test_reply<void>();
	DO_NOT_CALL_compilation_test_reply<JphPost*>();
	DO_NOT_CALL_compilation_test_reply<JphUser>();
	DO_NOT_CALL_compilation_test_reply<JphUserSimple>();
	DO_NOT_CALL_compilation_test_reply<JphPostSimple*>();
	DO_NOT_CALL_compilation_test_reply<QList<JphPost*>>();
	DO_NOT_CALL_compilation_test_reply<QList<JphUser>>();
	DO_NOT_CALL_compilation_test_reply<QList<JphUserSimple>>();
	DO_NOT_CALL_compilation_test_reply<QList<JphPostSimple*>>();
	DO_NOT_CALL_compilation_test_reply<Paging<JphPost*>>();
	DO_NOT_CALL_compilation_test_reply<Paging<JphUser>>();
	DO_NOT_CALL_compilation_test_reply<Paging<JphUserSimple>>();
	DO_NOT_CALL_compilation_test_reply<Paging<JphPostSimple*>>();

	DO_NOT_CALL_compilation_test_paging<JphPost*>();
	DO_NOT_CALL_compilation_test_paging<JphPostSimple*>();
	DO_NOT_CALL_compilation_test_paging<JphUser>();
	DO_NOT_CALL_compilation_test_paging<JphUserSimple>();

	DO_NOT_CALL_compilation_test_class<JphPost*>();//object
	DO_NOT_CALL_compilation_test_class<JphUser>();//gadget
	DO_NOT_CALL_compilation_test_class<JphUserSimple>();//gadget
	DO_NOT_CALL_compilation_test_class<JphPostSimple*>();//object

	JphUserSimple s;
	s.hasExtension();
	s.isExtended();
	s.currentExtended();
	s.extend(nullptr);
	s.extend(nullptr, {});
	s.extend(nullptr, nullptr, {});

	auto p = new JphPostSimple{};
	p->hasExtension();
	p->isExtended();
	p->currentExtended();
	p->extend(nullptr);
	p->extend(nullptr, {});
	p->extend(nullptr, nullptr, {});
}

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
