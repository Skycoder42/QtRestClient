#include "tst_global.h"
using namespace QtRestClient;

class BasicJsonTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testRestChain();

private:
	RestClient *client;
};

void BasicJsonTest::initTestCase()
{
	client = new RestClient(this);
	client->setBaseUrl(QStringLiteral("https://jsonplaceholder.typicode.com"));
}

void BasicJsonTest::cleanupTestCase()
{
	client->deleteLater();
	client = nullptr;
}

void BasicJsonTest::testRestChain()
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

QTEST_MAIN(BasicJsonTest)

#include "tst_basicjson.moc"
