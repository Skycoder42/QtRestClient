#include "jphuser.h"
#include "testlib.h"

using namespace QtRestClient;

class IntegrationTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testJsonChain();
	void testQObjectChain();
	void testQObjectListChain();
	void testQObjectPagingChain();

private:
	HttpServer *server;
	RestClient *client;
};

void IntegrationTest::initTestCase()
{
#ifdef Q_OS_UNIX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5RestClient"));
#endif
	QJsonSerializer::registerListConverters<JphPost*>();
	server = new HttpServer(this);
	server->verifyRunning();
	server->setAdvancedData();
	client = Testlib::createClient(this);
	client->setBaseUrl(QStringLiteral("http://localhost:%1").arg(server->serverPort()));
}

void IntegrationTest::cleanupTestCase()
{
	if(client) {
		client->deleteLater();
		client = nullptr;
	}
}

void IntegrationTest::testJsonChain()
{
	QJsonObject object;
	object["id"] = 1;
	object["userId"] = 42;
	object["title"] = "baum";
	object["body"] = "baum";

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->callJson(RestClass::PutVerb, "1", object);
	reply->onSucceeded([&](int code, QJsonObject data){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(data, object);
	});
	reply->onAllErrors([&](QString error, int code, RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});

	object["id"] = 1;
	QSignalSpy deleteSpy(reply, &RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
}

void IntegrationTest::testQObjectChain()
{
	auto object = new JphPost(2, 42, "baum", "baum", this);

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->put<JphPost*>("2", object);
	reply->onSucceeded([&](int code, JphPost *data){
		called = true;
		QCOMPARE(code, 200);
		QVERIFY(JphPost::equals(data, object));
		data->deleteLater();
	});
	reply->onAllErrors([&](QString error, int code, RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});

	QSignalSpy deleteSpy(reply, &RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
	object->deleteLater();
}

void IntegrationTest::testQObjectListChain()
{
	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->get<QList<JphPost*>>();
	reply->onSucceeded([&](int code, QList<JphPost*> data){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(data.size(), 100);
		qDeleteAll(data);
	});
	reply->onAllErrors([&](QString error, int code, RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});

	QSignalSpy deleteSpy(reply, &RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
}

void IntegrationTest::testQObjectPagingChain()
{
	auto pagingClass = client->createClass("pages", client);

	int count = 0;

	auto reply = pagingClass->get<Paging<JphPost*>>("0");
	reply->iterate([&](JphPost* data, int index){
		auto ok = false;
		[&](){
			QCOMPARE(index, count);
			QCOMPARE(data->id, count++);
			ok = true;
		}();
		data->deleteLater();
		return ok;
	});
	reply->onAllErrors([&](QString error, int code, RestReply::ErrorType){
		count = 142;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});

	while(count < 100)
		QCoreApplication::processEvents();
	QCoreApplication::processEvents();

	pagingClass->deleteLater();
}

template <typename T>
static void DO_NOT_CALL_compilation_test_template()
{
	T object = T();
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
	postClass->post<T>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->post<T>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->post<T>();
	postClass->post<T>(object);
	postClass->post<T>(list);
	postClass->post<T>(RestClass::concatParams("baum", 42));
	postClass->post<T>(object, RestClass::concatParams("baum", 42));
	postClass->post<T>(list, RestClass::concatParams("baum", 42));
	postClass->post<T>(QUrl("test"));
	postClass->post<T>(QUrl("test"), object);
	postClass->post<T>(QUrl("test"), list);
	postClass->post<T>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->post<T>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->post<T>(QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->put<T>(QStringLiteral("test"));
	postClass->put<T>(QStringLiteral("test"), object);
	postClass->put<T>(QStringLiteral("test"), list);
	postClass->put<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->put<T>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->put<T>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->put<T>();
	postClass->put<T>(object);
	postClass->put<T>(list);
	postClass->put<T>(RestClass::concatParams("baum", 42));
	postClass->put<T>(object, RestClass::concatParams("baum", 42));
	postClass->put<T>(list, RestClass::concatParams("baum", 42));
	postClass->put<T>(QUrl("test"));
	postClass->put<T>(QUrl("test"), object);
	postClass->put<T>(QUrl("test"), list);
	postClass->put<T>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->put<T>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->put<T>(QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->deleteResource<T>(QStringLiteral("test"));
	postClass->deleteResource<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->deleteResource<T>();
	postClass->deleteResource<T>(RestClass::concatParams("baum", 42));
	postClass->deleteResource<T>(QUrl("test"));
	postClass->deleteResource<T>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->patch<T>(QStringLiteral("test"));
	postClass->patch<T>(QStringLiteral("test"), object);
	postClass->patch<T>(QStringLiteral("test"), list);
	postClass->patch<T>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->patch<T>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->patch<T>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->patch<T>();
	postClass->patch<T>(object);
	postClass->patch<T>(list);
	postClass->patch<T>(RestClass::concatParams("baum", 42));
	postClass->patch<T>(object, RestClass::concatParams("baum", 42));
	postClass->patch<T>(list, RestClass::concatParams("baum", 42));
	postClass->patch<T>(QUrl("test"));
	postClass->patch<T>(QUrl("test"), object);
	postClass->patch<T>(QUrl("test"), list);
	postClass->patch<T>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->patch<T>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->patch<T>(QUrl("test"), list, RestClass::concatParams("baum", 42));
}

static void DO_NOT_CALL_compilation_test()
{
	DO_NOT_CALL_compilation_test_template<JphPost*>();//object
	DO_NOT_CALL_compilation_test_template<JphUser>();//gadget
	DO_NOT_CALL_compilation_test_template<JphUserSimple>();//gadget
	JphUserSimple s;
	s.extend(nullptr);
}

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
