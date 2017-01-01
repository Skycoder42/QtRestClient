#include "tst_global.h"

#include "testobject.h"
#include <QJsonValue>

class RestObjectTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testRestMacros();
};

void RestObjectTest::initTestCase()
{
	QtRestClient::registerListConverters<bool>();
}

void RestObjectTest::cleanupTestCase()
{
}

void RestObjectTest::testRestMacros()
{
	TestObject testObject;
	testObject.id = 42;
	testObject.name = "baum";
	testObject.stateMap = {true, false, false, true};
	testObject.child = new TestObject(&testObject);
	testObject.child->id = 13;

	auto meta = static_cast<QtRestClient::RestObject*>(&testObject)->metaObject();

	//verfiy all metaobject conversions work
	auto idPos = meta->indexOfProperty("id");
	QVERIFY(meta->property(idPos).read(&testObject).convert(QMetaType::Int));
	auto namePos = meta->indexOfProperty("name");
	QVERIFY(meta->property(namePos).read(&testObject).convert(QMetaType::QString));
	auto stateMapPos = meta->indexOfProperty("stateMap");
	QVERIFY(meta->property(stateMapPos).read(&testObject).convert(QMetaType::QVariantList));
	auto childPos = meta->indexOfProperty("child");
	QVERIFY(meta->property(childPos).read(&testObject).convert(QtRestClient::RestObject::metaId()));
}

QTEST_MAIN(RestObjectTest)

#include "tst_restobject.moc"
