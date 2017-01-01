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

	void testSerialization_data();
	void testSerialization();

private:
	QtRestClient::JsonSerializer *ser;
};

void RestObjectTest::initTestCase()
{
	QtRestClient::registerListConverters<bool>();
	ser = new QtRestClient::JsonSerializer(this);
}

void RestObjectTest::cleanupTestCase()
{
	ser->deleteLater();
	ser = nullptr;
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
	QVERIFY(meta->property(childPos).read(&testObject).convert(qMetaTypeId<QtRestClient::RestObject*>()));
}

void RestObjectTest::testSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");

	QTest::newRow("default") << new TestObject(this)
							 << QJsonObject({
												{"id", -1},
												{"name", QJsonValue::Null},
												{"stateMap", QJsonArray()},
												{"child", QJsonValue::Null}
											});

	QTest::newRow("basic") << new TestObject(42, "baum", {}, -1, this)
						   << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonValue::Null}
										  });

	QTest::newRow("list") << new TestObject(42, "baum", {true, false, false, true}, -1, this)
						  << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(false), QJsonValue(true)})},
											 {"child", QJsonValue::Null}
										 });

	QTest::newRow("child") << new TestObject(42, "baum", {}, 13, this)
						   << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonObject({
												   {"id", 13},
												   {"name", QJsonValue::Null},
												   {"stateMap", QJsonArray()},
												   {"child", QJsonValue::Null}
											   })}
										  });

	auto parent = new TestObject(42, "baum", {}, -1, this);
	parent->child = new TestObject(13, "lucky", {}, 7, this);
	QTest::newRow("recursive") << parent
							   << QJsonObject({
												  {"id", 42},
												  {"name", "baum"},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonObject({
													   {"id", 13},
													   {"name", "lucky"},
													   {"stateMap", QJsonArray()},
													   {"child", QJsonObject({
															{"id", 7},
															{"name", QJsonValue::Null},
															{"stateMap", QJsonArray()},
															{"child", QJsonValue::Null}
														})}
												   })}
											  });

	QTest::newRow("full") << new TestObject(42, "baum", {true, false, true, false}, 13, this)
						  << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(true), QJsonValue(false)})},
											 {"child", QJsonObject({
												  {"id", 13},
												  {"name", QJsonValue::Null},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonValue::Null}
											  })}
										 });
}

void RestObjectTest::testSerialization()
{
	QFETCH(TestObject*, object);
	QFETCH(QJsonObject, result);

	auto ok = false;
	auto res = ser->serialize(object, &ok);
	QVERIFY(ok);
	QCOMPARE(res, result);
}

QTEST_MAIN(RestObjectTest)

#include "tst_restobject.moc"
