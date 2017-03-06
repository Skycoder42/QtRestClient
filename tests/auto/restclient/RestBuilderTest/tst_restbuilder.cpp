#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <User.h>
#include <Post.h>

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
};

RestBuilderTest::RestBuilderTest()
{
}

void RestBuilderTest::initTestCase()
{
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
}

QTEST_MAIN(RestBuilderTest)

#include "tst_restbuilder.moc"
