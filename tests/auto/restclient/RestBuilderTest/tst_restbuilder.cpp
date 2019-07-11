#include "testlib.h"
#include <user.h>
#include <post.h>
#include <api_posts.h>
#include <test_api.h>
#include <simplepost.h>
using namespace TestSpace;

class RestBuilderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testCustomCompiledObject();
	void testCustomCompiledGadget();
	void testCustomCompiledApi();
	void testCustomCompiledApiPosts();

private:
	HttpServer *server;
};

void RestBuilderTest::initTestCase()
{
	server = new HttpServer(45715, this);
	server->verifyRunning();

	QJsonObject root;
	QJsonObject vRoot;
	QJsonArray posts;
	for(auto i = 0; i < 100; i++) {
		posts.append(QJsonObject {
						 {QStringLiteral("id"), i},
						 {QStringLiteral("user"), QJsonObject {
							  {QStringLiteral("id"), qCeil(i/2.0)},
							  {QStringLiteral("name"), QStringLiteral("user%1").arg(qCeil(i/2.0))},
						  }},
						 {QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
						 {QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
					 });
	}
	vRoot[QStringLiteral("posts")] = posts;
	root[QStringLiteral("v1")] = vRoot;
	server->setData(root);

	qRegisterMetaType<QtRestClient::RestReply::ErrorType>();
}

void RestBuilderTest::cleanupTestCase()
{
	server->deleteLater();
	server = nullptr;
}

void RestBuilderTest::testCustomCompiledObject()
{
	User user;

	QSignalSpy idSpy(&user, &User::idChanged);
	QSignalSpy nameSpy(&user, &User::nameChanged);

	QCOMPARE(idSpy.count(), 0);
	user.setId(42);
	QCOMPARE(idSpy.count(), 1);
	QCOMPARE(nameSpy.count(), 0);
	user.setName("baum");
	QCOMPARE(nameSpy.count(), 1);
	user.setGender(User::GenderType::Male);
	user.setTitle(User::Doctor | User::Professor);

	QCOMPARE(user.property("id").toInt(), 42);
	QCOMPARE(user.property("name").toString(), QStringLiteral("baum"));
	QCOMPARE(user.property("gender").value<User::GenderType>(), User::GenderType::Male);
	QCOMPARE(user.property("title").toInt(), static_cast<int>(User::Doctor | User::Professor));

	User user2 {42, QStringLiteral("baum"), User::GenderType::Male, User::Doctor | User::Professor};
	QVERIFY(user.equals(&user2));
}

void RestBuilderTest::testCustomCompiledGadget()
{
	Post post;

	post.setId(42);
	post.setTitle(QStringLiteral("baum"));
	post.setVersion(QVersionNumber{1, 1, 0});
	post.setBody(QStringLiteral("baum == 42"));
	QCOMPARE(post.user(), nullptr);

	Post post2 {42, QStringLiteral("baum"), QVersionNumber{1, 1, 0}, QStringLiteral("baum == 42")};
	QCOMPARE(post2, post);
}

void RestBuilderTest::testCustomCompiledApi()
{
	// test eveything is there
	auto api = new TestApi(this);
	QVERIFY(api->restClient());
	QVERIFY(api->restClass());
	QVERIFY(api->posts());
	api->deleteLater();

	//same for factory creation
	auto t1 = TestApi::factory().instance(this);
	QVERIFY(t1);
	t1->deleteLater();
	auto t2 = TestApi::factory().posts().instance(this);
	QVERIFY(t2);
	t2->deleteLater();
}

void RestBuilderTest::testCustomCompiledApiPosts()
{
	auto api = new TestApi(this);

	bool called = false;
	auto reply = api->posts()->listPosts();
	reply->onSucceeded([&](int code, QList<Post> posts){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(posts.size(), 100);
	});
	reply->onAllErrors([&](QString error, int, QtRestClient::RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error));
	});

	QSignalSpy deleteSpy(reply, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy.wait());
	QVERIFY(called);

	called = false;
	auto reply2 = api->posts()->post(42);
	reply2->onSucceeded([&](int code, Post post){
		called = true;
		QCOMPARE(code, 200);
		QCOMPARE(post.id(), 42);
		QVERIFY(post.user());
		QCOMPARE(post.user()->id(), 42/2);
		post.user()->deleteLater();
	});
	reply2->onAllErrors([&](QString error, int, QtRestClient::RestReply::ErrorType){
		called = true;
		QFAIL(qUtf8Printable(error));
	});

	QSignalSpy deleteSpy2(reply2, &QtRestClient::RestReply::destroyed);
	QVERIFY(deleteSpy2.wait());
	QVERIFY(called);

	called = false;
	QCoreApplication::processEvents();
	QSignalSpy errorSpy(api->posts(), &PostClass::apiError);
	api->posts()->setErrorTranslator([&](QObject*,int){
		called = true;
		return QString();
	});
	auto reply3 = api->posts()->post(4242);
	reply3->onSucceeded([&](int, Post){
		called = true;
		QFAIL("Expected to fail!");
	});
	QSignalSpy deleteSpy3(reply3, &QtRestClient::RestReply::destroyed);

	QVERIFY(errorSpy.wait());
	QCOMPARE(errorSpy[0][2].toInt(), static_cast<int>(QtRestClient::RestReply::FailureError));
	QCOMPARE(errorSpy[0][1].toInt(), 404);

	QVERIFY(deleteSpy3.wait());
	QVERIFY(called);
}

QTEST_MAIN(RestBuilderTest)

#include "tst_restbuilder.moc"
