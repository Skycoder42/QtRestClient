#include "tst_global.h"
#include <user.h>
#include <post.h>
#include <api_posts.h>
#include <test_api.h>

class RestBuilderTest : public QObject
{
	Q_OBJECT

public:
	RestBuilderTest();

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testCustomCompiledObject();
	void testCustomCompiledGadget();
	void testCustomCompiledApi();
};

RestBuilderTest::RestBuilderTest()
{
}

void RestBuilderTest::initTestCase()
{
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5RestClient"));
	QCoreApplication::processEvents();
	QJsonSerializer::registerListConverters<Post>();
	QJsonSerializer::registerListConverters<User*>();
	initTestJsonServer("./advanced-test-db.js");
}

void RestBuilderTest::cleanupTestCase()
{
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

	QCOMPARE(user.property("id").toInt(), 42);
	QCOMPARE(user.property("name").toString(), QStringLiteral("baum"));
}

void RestBuilderTest::testCustomCompiledGadget()
{
	Post post;

	post.setId(42);
	post.setTitle("baum");
	post.setBody("baum == 42");
	QCOMPARE(post.user(), nullptr);
}

void RestBuilderTest::testCustomCompiledApi()
{
	// test eveything is there
	auto api = new TestApi(this);
	QVERIFY(api->restClient());
	QVERIFY(api->restClass());
	QVERIFY(api->posts());

	//same for factory creation
	auto t1 = TestApi::factory().instance(this);
	QVERIFY(t1);
	t1->deleteLater();
	auto t2 = TestApi::factory().posts().instance(this);
	QVERIFY(t2);
	t2->deleteLater();
}

QTEST_MAIN(RestBuilderTest)

#include "tst_restbuilder.moc"
