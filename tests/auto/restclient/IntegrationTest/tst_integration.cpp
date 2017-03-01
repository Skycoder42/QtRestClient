#include "tst_global.h"

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
	RestClient *client;
};

void IntegrationTest::initTestCase()
{
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5RestClient"));
	QJsonSerializer::registerListConverters<JphPost*>();
	initTestJsonServer("./advanced-test-db.js");
	client = new RestClient(this);
	client->setBaseUrl(QStringLiteral("http://localhost:3000"));
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
	object["userId"] = 42;
	object["title"] = "baum";
	object["body"] = "baum";

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->callJson(RestClass::PutVerb, "1", object);
	reply->enableAutoDelete();
	reply->onSucceeded([&](RestReply *rep, int code, QJsonObject data){
		called = true;
		QCOMPARE(rep, reply);
		QCOMPARE(code, 200);
		QCOMPARE(data, object);
	});
	reply->onAllErrors([&](RestReply *, QString error, int code, RestReply::ErrorType){
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
	reply->enableAutoDelete();
	reply->onSucceeded([&](RestReply *rep, int code, JphPost *data){
		called = true;
		QCOMPARE(rep, reply);
		QCOMPARE(code, 200);
		QVERIFY(JphPost::equals(data, object));
		data->deleteLater();
	});
	reply->onAllErrors([&](RestReply *, QString error, int code, RestReply::ErrorType){
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
	reply->enableAutoDelete();
	reply->onSucceeded([&](RestReply *rep, int code, QList<JphPost*> data){
		called = true;
		QCOMPARE(rep, reply);
		QCOMPARE(code, 200);
		QCOMPARE(data.size(), 100);
		qDeleteAll(data);
	});
	reply->onAllErrors([&](RestReply *, QString error, int code, RestReply::ErrorType){
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
	reply->enableAutoDelete();
	reply->iterate([&](Paging<JphPost*> *, JphPost* data, int index){
		auto ok = false;
		[&](){
			QCOMPARE(index, count++);
			QCOMPARE(data->id, count);
			ok = true;
		}();
		data->deleteLater();
		return ok;
	});
	reply->onAllErrors([&](RestReply *, QString error, int code, RestReply::ErrorType){
		count = 142;
		QFAIL(qUtf8Printable(error.isEmpty() ? QString::number(code) : error));
	});

	while(count < 100)
		QCoreApplication::processEvents();
	QCoreApplication::processEvents();

	pagingClass->deleteLater();
}

static void DO_NOT_CALL_compilation_test()
{
	JphPost *object = nullptr;
	QList<JphPost*> list;
	RestClass *postClass = nullptr;

	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"));
	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"), object);
	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"), list);
	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb);
	postClass->call<JphPost*>(RestClass::GetVerb, object);
	postClass->call<JphPost*>(RestClass::GetVerb, list);
	postClass->call<JphPost*>(RestClass::GetVerb, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, object, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, list, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"));
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"), object);
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"), list);
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->call<JphPost*>(RestClass::GetVerb, QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->get<JphPost*>(QStringLiteral("test"));
	postClass->get<JphPost*>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->get<JphPost*>();
	postClass->get<JphPost*>(RestClass::concatParams("baum", 42));
	postClass->get<JphPost*>(QUrl("test"));
	postClass->get<JphPost*>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->post<JphPost*>(QStringLiteral("test"));
	postClass->post<JphPost*>(QStringLiteral("test"), object);
	postClass->post<JphPost*>(QStringLiteral("test"), list);
	postClass->post<JphPost*>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>();
	postClass->post<JphPost*>(object);
	postClass->post<JphPost*>(list);
	postClass->post<JphPost*>(RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(object, RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(list, RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(QUrl("test"));
	postClass->post<JphPost*>(QUrl("test"), object);
	postClass->post<JphPost*>(QUrl("test"), list);
	postClass->post<JphPost*>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->post<JphPost*>(QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->put<JphPost*>(QStringLiteral("test"));
	postClass->put<JphPost*>(QStringLiteral("test"), object);
	postClass->put<JphPost*>(QStringLiteral("test"), list);
	postClass->put<JphPost*>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>();
	postClass->put<JphPost*>(object);
	postClass->put<JphPost*>(list);
	postClass->put<JphPost*>(RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(object, RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(list, RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(QUrl("test"));
	postClass->put<JphPost*>(QUrl("test"), object);
	postClass->put<JphPost*>(QUrl("test"), list);
	postClass->put<JphPost*>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->put<JphPost*>(QUrl("test"), list, RestClass::concatParams("baum", 42));

	postClass->deleteResource<JphPost*>(QStringLiteral("test"));
	postClass->deleteResource<JphPost*>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->deleteResource<JphPost*>();
	postClass->deleteResource<JphPost*>(RestClass::concatParams("baum", 42));
	postClass->deleteResource<JphPost*>(QUrl("test"));
	postClass->deleteResource<JphPost*>(QUrl("test"), RestClass::concatParams("baum", 42));

	postClass->patch<JphPost*>(QStringLiteral("test"));
	postClass->patch<JphPost*>(QStringLiteral("test"), object);
	postClass->patch<JphPost*>(QStringLiteral("test"), list);
	postClass->patch<JphPost*>(QStringLiteral("test"), RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(QStringLiteral("test"), object, RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(QStringLiteral("test"), list, RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>();
	postClass->patch<JphPost*>(object);
	postClass->patch<JphPost*>(list);
	postClass->patch<JphPost*>(RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(object, RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(list, RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(QUrl("test"));
	postClass->patch<JphPost*>(QUrl("test"), object);
	postClass->patch<JphPost*>(QUrl("test"), list);
	postClass->patch<JphPost*>(QUrl("test"), RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(QUrl("test"), object, RestClass::concatParams("baum", 42));
	postClass->patch<JphPost*>(QUrl("test"), list, RestClass::concatParams("baum", 42));
}

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
