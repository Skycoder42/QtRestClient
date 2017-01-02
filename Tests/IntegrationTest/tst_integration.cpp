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

	auto reply = postClass->call(RestClass::PutVerb, "1", object);
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

	auto reply = postClass->call<JphPost>(RestClass::PutVerb, "1", object);
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

QTEST_MAIN(IntegrationTest)

#include "tst_integration.moc"
