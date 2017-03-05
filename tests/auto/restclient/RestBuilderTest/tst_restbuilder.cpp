#include <QString>
#include <QtTest>
#include <QCoreApplication>

class RestBuilderTest : public QObject
{
	Q_OBJECT

public:
	RestBuilderTest();

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testCustomCompiler_data();
	void testCustomCompiler();
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

void RestBuilderTest::testCustomCompiler_data()
{
	QTest::addColumn<QString>("data");
	QTest::newRow("0") << QString();
}

void RestBuilderTest::testCustomCompiler()
{
	QFETCH(QString, data);
	QVERIFY2(true, "Failure");
}

QTEST_MAIN(RestBuilderTest)

#include "tst_restbuilder.moc"
