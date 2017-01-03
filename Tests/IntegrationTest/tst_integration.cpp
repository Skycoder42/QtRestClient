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
	client = new RestClient(this);
	client->setBaseUrl(QStringLiteral("https://jsonplaceholder.typicode.com"));
}

void IntegrationTest::cleanupTestCase()
{
	client->deleteLater();
	client = nullptr;
}

void IntegrationTest::testJsonChain()
{
	QJsonObject object;
	object["userId"] = 42;
	object["id"] = 1;
	object["title"] = "baum";
	object["body"] = "baum";

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->put("1", object);
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

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	postClass->deleteLater();
}

void IntegrationTest::testRestObjectChain()
{
	RestObject *object = new JphPost(1, 42, "baum", "baum", this);

	auto postClass = client->createClass("posts", client);

	bool called = false;

	auto reply = postClass->put<JphPost>("1", object);
	reply->enableAutoDelete();
	reply->onSucceeded([&](QtRestClient::GenericRestReply<JphPost> *rep, int code, JphPost *data){
		called = true;
		[&](){
			QCOMPARE(rep, reply);
			QCOMPARE(code, 200);
			QVERIFY(RestObject::equals(data, object));
		}();
		return false;
	});
	reply->onFailed([&](QtRestClient::GenericRestReply<JphPost> *, int code, RestObject *){
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
	reply->onSucceeded([&](QtRestClient::GenericRestReply<QList<JphPost>> *rep, int code, QList<JphPost*> data){
		called = true;
		[&](){
			QCOMPARE(rep, reply);
			QCOMPARE(code, 200);
			QCOMPARE(data.size(), 100);
		}();
		return false;
	});
	reply->onFailed([&](QtRestClient::GenericRestReply<QList<JphPost>> *, int code, RestObject *){
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
	RestObject *object = nullptr;
	RestClass *postClass = nullptr;

	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"));
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), object);
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb);
	postClass->call<JphPost>(RestClass::GetVerb, object);
	postClass->call<JphPost>(RestClass::GetVerb, RestClass::concatParameters("baum", 42));
	postClass->call<JphPost>(RestClass::GetVerb, object, RestClass::concatParameters("baum", 42));

	postClass->get(QStringLiteral("test"));
	postClass->get<JphPost>(QStringLiteral("test"));
	postClass->get<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->get();
	postClass->get<JphPost>();
	postClass->get<JphPost>(RestClass::concatParameters("baum", 42));

	postClass->post(QStringLiteral("test"));
	postClass->post(QStringLiteral("test"), QJsonObject());
	postClass->post(QStringLiteral("test"), QJsonArray());
	postClass->post<JphPost>(QStringLiteral("test"));
	postClass->post<JphPost>(QStringLiteral("test"), object);
	postClass->post<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->post();
	postClass->post(QJsonObject());
	postClass->post(QJsonArray());
	postClass->post<JphPost>();
	postClass->post<JphPost>(object);
	postClass->post<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->post<JphPost>(object, RestClass::concatParameters("baum", 42));

	postClass->put(QStringLiteral("test"));
	postClass->put(QStringLiteral("test"), QJsonObject());
	postClass->put(QStringLiteral("test"), QJsonArray());
	postClass->put<JphPost>(QStringLiteral("test"));
	postClass->put<JphPost>(QStringLiteral("test"), object);
	postClass->put<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->put();
	postClass->put(QJsonObject());
	postClass->put(QJsonArray());
	postClass->put<JphPost>();
	postClass->put<JphPost>(object);
	postClass->put<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->put<JphPost>(object, RestClass::concatParameters("baum", 42));

	postClass->deleteResource(QStringLiteral("test"));
	postClass->deleteResource<JphPost>(QStringLiteral("test"));
	postClass->deleteResource<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->deleteResource();
	postClass->deleteResource<JphPost>();
	postClass->deleteResource<JphPost>(RestClass::concatParameters("baum", 42));

	postClass->patch(QStringLiteral("test"));
	postClass->patch(QStringLiteral("test"), QJsonObject());
	postClass->patch(QStringLiteral("test"), QJsonArray());
	postClass->patch<JphPost>(QStringLiteral("test"));
	postClass->patch<JphPost>(QStringLiteral("test"), object);
	postClass->patch<JphPost>(QStringLiteral("test"), RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(QStringLiteral("test"), object, RestClass::concatParameters("baum", 42));
	postClass->patch();
	postClass->patch(QJsonObject());
	postClass->patch(QJsonArray());
	postClass->patch<JphPost>();
	postClass->patch<JphPost>(object);
	postClass->patch<JphPost>(RestClass::concatParameters("baum", 42));
	postClass->patch<JphPost>(object, RestClass::concatParameters("baum", 42));
}

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
