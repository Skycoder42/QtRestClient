#include "tst_global.h"

using namespace QtRestClient;

class IntegrationTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testJsonChain();
	void testRestObjectChain();
	void testRestObjectListChain();

private:
	RestClient *client;
};

void IntegrationTest::initTestCase()
{
	initTestJsonServer();
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
	reply->onSucceeded([&](QtRestClient::RestReply *rep, int code, QJsonObject data){
		called = true;
		QCOMPARE(rep, reply);
		QCOMPARE(code, 200);
		QCOMPARE(data, object);
	});
	reply->onFailed([&](QtRestClient::RestReply *, int code, QJsonObject){
		called = true;
		QFAIL(QByteArray::number(code).constData());
	});
	reply->onError([&](QtRestClient::RestReply *, QString error, int, QtRestClient::RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error));
	});

	object["id"] = 1;
	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
}

void IntegrationTest::testRestObjectChain()
{
	RestObject *object = new JphPost(2, 42, "baum", "baum", this);

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->put<JphPost>("2", object);
	reply->enableAutoDelete();
	reply->onSucceeded([&](QtRestClient::RestReply *rep, int code, JphPost *data){
		called = true;
		[&](){
			QCOMPARE(rep, reply);
			QCOMPARE(code, 200);
			QVERIFY(RestObject::equals(data, object));
		}();
		return false;
	});
	reply->onFailed([&](QtRestClient::RestReply *, int code, RestObject *){
		called = true;
		[&](){
			QFAIL(QByteArray::number(code).constData());
		}();
		return false;
	});
	reply->onError([&](QtRestClient::RestReply *, QString error, int, QtRestClient::RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error));
	});

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
	object->deleteLater();
}

void IntegrationTest::testRestObjectListChain()
{
	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->get<QList<JphPost>>();
	reply->enableAutoDelete();
	reply->onSucceeded([&](QtRestClient::RestReply *rep, int code, QList<JphPost*> data){
		called = true;
		[&](){
			QCOMPARE(rep, reply);
			QCOMPARE(code, 200);
			QCOMPARE(data.size(), 10);
		}();
		return false;
	});
	reply->onFailed([&](QtRestClient::RestReply *, int code, RestObject *){
		called = true;
		[&](){
			QFAIL(QByteArray::number(code).constData());
		}();
		return false;
	});
	reply->onError([&](QtRestClient::RestReply *, QString error, int, QtRestClient::RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error));
	});

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
}

static void DO_NOT_CALL_compilation_test()
{
	JphPost *object = nullptr;
	QList<JphPost*> list;
	RestClass *postClass = nullptr;

	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"));
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), object);
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), list);
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), list, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb);
	postClass->call<JphPost>(RestClass::GetVerb, object);
	postClass->call<JphPost>(RestClass::GetVerb, list);
	postClass->call<JphPost>(RestClass::GetVerb, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, object, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, list, RestClass::concatParameters("baum", 42));

	postClass->get<JphPost>(QStringLiteral("test"));
	postClass->get<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->get<JphPost>();
	postClass->get<JphPost>(RestClass::concatParameters("baum", 42));

	postClass->post<JphPost>(QStringLiteral("test"));
	postClass->post<JphPost>(QStringLiteral("test"), object);
	postClass->post<JphPost>(QStringLiteral("test"), list);
	postClass->post<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(QStringLiteral("test"), list, RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>();
	postClass->post<JphPost>(object);
	postClass->post<JphPost>(list);
	postClass->post<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(object, RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(list, RestClass::concatParameters("baum", 42));

	postClass->put<JphPost>(QStringLiteral("test"));
	postClass->put<JphPost>(QStringLiteral("test"), object);
	postClass->put<JphPost>(QStringLiteral("test"), list);
	postClass->put<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(QStringLiteral("test"), list, RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>();
	postClass->put<JphPost>(object);
	postClass->put<JphPost>(list);
	postClass->put<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(object, RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(list, RestClass::concatParameters("baum", 42));

	postClass->deleteResource<JphPost>(QStringLiteral("test"));
	postClass->deleteResource<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->deleteResource<JphPost>();
	postClass->deleteResource<JphPost>(RestClass::concatParameters("baum", 42));

	postClass->patch<JphPost>(QStringLiteral("test"));
	postClass->patch<JphPost>(QStringLiteral("test"), object);
	postClass->patch<JphPost>(QStringLiteral("test"), list);
	postClass->patch<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(QStringLiteral("test"), list, RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>();
	postClass->patch<JphPost>(object);
	postClass->patch<JphPost>(list);
	postClass->patch<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(object, RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(list, RestClass::concatParameters("baum", 42));
}

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
