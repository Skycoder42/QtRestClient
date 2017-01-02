#include "brokentestobject.h"
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

	void testEquality_data();
	void testEquality();

	void testSerialization_data();
	void testSerialization();

	void testDeserialization_data();
	void testDeserialization();

private:
	QtRestClient::JsonSerializer *ser;
};

void RestObjectTest::initTestCase()
{
	QtRestClient::registerListConverters<bool>();
	QtRestClient::registerListConverters<TestObject*>();
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
	auto relativesPos = meta->indexOfProperty("relatives");
	QVERIFY(meta->property(relativesPos).read(&testObject).convert(QMetaType::QVariantList));

	QVERIFY(meta->indexOfProperty("__qtrc_ro_olp_relatives") != -1);
}

void RestObjectTest::testEquality_data()
{
	QTest::addColumn<QtRestClient::RestObject*>("object1");
	QTest::addColumn<QtRestClient::RestObject*>("object2");
	QTest::addColumn<bool>("areEqual");

	auto ident = new TestObject(this);
	QTest::newRow("ident") << (QtRestClient::RestObject*)ident
						   << (QtRestClient::RestObject*)ident
						   << true;

	QTest::newRow("null") << (QtRestClient::RestObject*)new TestObject(this)
						  << (QtRestClient::RestObject*)nullptr
						  << false;

	QTest::newRow("differentClasses") << (QtRestClient::RestObject*)new TestObject(this)
									  << (QtRestClient::RestObject*)new QtRestClient::RestObject(this)
									  << false;

	QTest::newRow("equalNoChild") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, -1, this)
								  << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, -1, this)
								  << true;

	QTest::newRow("equalChild") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, 13, this)
								  << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, 13, this)
								  << true;

	auto parent1 = new TestObject(42, "baum", {}, -1, this);
	parent1->child = new TestObject(13, "lucky", {}, 7, parent1);
	auto parent2 = new TestObject(42, "baum", {}, -1, this);
	parent2->child = new TestObject(13, "lucky", {}, 7, parent1);
	QTest::newRow("equalChildRecursive") << (QtRestClient::RestObject*)parent1
										 << (QtRestClient::RestObject*)parent2
										 << true;

	QTest::newRow("unequalProperty") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, -1, this)
									 << (QtRestClient::RestObject*)new TestObject(42, "42", {true, false, true}, -1, this)
									 << false;

	QTest::newRow("unequalList") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, -1, this)
								 << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, true, true}, -1, this)
								 << false;

	QTest::newRow("unequalChild") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, 13, this)
								  << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, 14, this)
								  << false;

	QTest::newRow("unequalChildNull") << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, 13, this)
									  << (QtRestClient::RestObject*)new TestObject(42, "baum", {true, false, true}, -1, this)
									  << false;
}

void RestObjectTest::testEquality()
{
	QFETCH(QtRestClient::RestObject*, object1);
	QFETCH(QtRestClient::RestObject*, object2);
	QFETCH(bool, areEqual);

	QCOMPARE(object1->equals(object2), areEqual);
	if(object2 != object1)
		object2->deleteLater();
	object1->deleteLater();
}

void RestObjectTest::testSerialization_data()
{
	QTest::addColumn<TestObject*>("object");
	QTest::addColumn<QJsonObject>("result");
	QTest::addColumn<bool>("shouldFail");

	QTest::newRow("default") << new TestObject(this)
							 << QJsonObject({
												{"id", -1},
												{"name", QString()},
												{"stateMap", QJsonArray()},
												{"child", QJsonValue::Null},
												{"relatives", QJsonArray()}
											})
							 << false;

	QTest::newRow("basic") << new TestObject(42, "baum", {}, -1, this)
						   << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonValue::Null},
											  {"relatives", QJsonArray()}
										  })
						   << false;

	QTest::newRow("list") << new TestObject(42, "baum", {true, false, false, true}, -1, this)
						  << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(false), QJsonValue(true)})},
											 {"child", QJsonValue::Null},
											 {"relatives", QJsonArray()}
										 })
						  << false;

	QTest::newRow("child") << new TestObject(42, "baum", {}, 13, this)
						   << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonObject({
												   {"id", 13},
												   {"name", QString()},
												   {"stateMap", QJsonArray()},
												   {"child", QJsonValue::Null},
												   {"relatives", QJsonArray()}
											   })},
											  {"relatives", QJsonArray()}
										  })
						   << false;

	auto parent = new TestObject(42, "baum", {}, -1, this);
	parent->child = new TestObject(13, "lucky", {}, 7, parent);
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
															{"name", QString()},
															{"stateMap", QJsonArray()},
															{"child", QJsonValue::Null},
															{"relatives", QJsonArray()}
														})},
													   {"relatives", QJsonArray()}
												   })},
												  {"relatives", QJsonArray()}
											  })
							   << false;

	QTest::newRow("full") << new TestObject(42, "baum", {true, false, true, false}, 13, this)
						  << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(true), QJsonValue(false)})},
											 {"child", QJsonObject({
												  {"id", 13},
												  {"name", QString()},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonValue::Null},
												  {"relatives", QJsonArray()}
											  })},
											 {"relatives", QJsonArray()}
										 })
						  << false;

	auto relator = new TestObject(42, "baum", {}, -1, this);
	relator->relatives.append(new TestObject(13, "lucky", {}, -1, relator));
	relator->relatives.append(new TestObject(7, "magically", {}, -1, relator));
	relator->relatives.append(new TestObject(3, "trinity", {}, -1, relator));
	QTest::newRow("relatives") << relator
							   << QJsonObject({
												  {"id", 42},
												  {"name", "baum"},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonValue::Null},
												  {"relatives", QJsonArray({
													   QJsonObject({
														   {"id", 13},
														   {"name", "lucky"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   }),
													   QJsonObject({
														   {"id", 7},
														   {"name", "magically"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   }),
													   QJsonObject({
														   {"id", 3},
														   {"name", "trinity"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   })
												   })}
											  })
							   << false;

	QTest::newRow("invalidType") << (TestObject*)new BrokenTestObject(this)
								 << QJsonObject({
													{"id", -1},
													{"name", QString()},
													{"stateMap", QJsonArray()},
													{"child", QJsonValue::Null},
													{"relatives", QJsonArray()},
													{"broken", QJsonValue::Null}
												})
								 << true;
}

void RestObjectTest::testSerialization()
{
	QFETCH(TestObject*, object);
	QFETCH(QJsonObject, result);
	QFETCH(bool, shouldFail);

	if(shouldFail)
		QVERIFY_EXCEPTION_THROWN(ser->serialize(object), QtRestClient::SerializerException);
	else
		QCOMPARE(ser->serialize(object), result);

	object->deleteLater();
}

void RestObjectTest::testDeserialization_data()
{
	QTest::addColumn<QJsonObject>("data");
	QTest::addColumn<TestObject*>("result");
	QTest::addColumn<bool>("shouldFail");

	QTest::newRow("default") << QJsonObject({
												{"id", -1},
												{"name", QString()},
												{"stateMap", QJsonArray()},
												{"child", QJsonValue::Null},
												{"relatives", QJsonArray()}
											})
							 << new TestObject(this)
							 << false;

	QTest::newRow("basic") << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonValue::Null},
											  {"relatives", QJsonArray()}
										  })
						   << new TestObject(42, "baum", {}, -1, this)
						   << false;

	QTest::newRow("list") << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(false), QJsonValue(true)})},
											 {"child", QJsonValue::Null},
											 {"relatives", QJsonArray()}
										 })
						  << new TestObject(42, "baum", {true, false, false, true}, -1, this)
						  << false;

	QTest::newRow("child") << QJsonObject({
											  {"id", 42},
											  {"name", "baum"},
											  {"stateMap", QJsonArray()},
											  {"child", QJsonObject({
												   {"id", 13},
												   {"name", QString()},
												   {"stateMap", QJsonArray()},
												   {"child", QJsonValue::Null},
												   {"relatives", QJsonArray()}
											   })},
											  {"relatives", QJsonArray()}
										  })
						   << new TestObject(42, "baum", {}, 13, this)
						   << false;

	auto parent = new TestObject(42, "baum", {}, -1, this);
	parent->child = new TestObject(13, "lucky", {}, 7, parent);
	QTest::newRow("recursive") << QJsonObject({
												  {"id", 42},
												  {"name", "baum"},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonObject({
													   {"id", 13},
													   {"name", "lucky"},
													   {"stateMap", QJsonArray()},
													   {"child", QJsonObject({
															{"id", 7},
															{"name", QString()},
															{"stateMap", QJsonArray()},
															{"child", QJsonValue::Null},
															{"relatives", QJsonArray()}
														})},
													   {"relatives", QJsonArray()}
												   })},
												  {"relatives", QJsonArray()}
											  })
							   << parent
							   << false;

	QTest::newRow("full") << QJsonObject({
											 {"id", 42},
											 {"name", "baum"},
											 {"stateMap", QJsonArray({QJsonValue(true), QJsonValue(false), QJsonValue(true), QJsonValue(false)})},
											 {"child", QJsonObject({
												  {"id", 13},
												  {"name", QString()},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonValue::Null},
												  {"relatives", QJsonArray()}
											  })},
											 {"relatives", QJsonArray()}
										 })
						  << new TestObject(42, "baum", {true, false, true, false}, 13, this)
						  << false;

	auto relator = new TestObject(42, "baum", {}, -1, this);
	relator->relatives.append(new TestObject(13, "lucky", {}, -1, relator));
	relator->relatives.append(new TestObject(7, "magically", {}, -1, relator));
	relator->relatives.append(new TestObject(3, "trinity", {}, -1, relator));
	QTest::newRow("relatives") << QJsonObject({
												  {"id", 42},
												  {"name", "baum"},
												  {"stateMap", QJsonArray()},
												  {"child", QJsonValue::Null},
												  {"relatives", QJsonArray({
													   QJsonObject({
														   {"id", 13},
														   {"name", "lucky"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   }),
													   QJsonObject({
														   {"id", 7},
														   {"name", "magically"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   }),
													   QJsonObject({
														   {"id", 3},
														   {"name", "trinity"},
														   {"stateMap", QJsonArray()},
														   {"child", QJsonValue::Null},
														   {"relatives", QJsonArray()}
													   })
												   })}
											  })
							   << relator
							   << false;

	auto dynT = new TestObject(42, "baum", {}, -1, this);
	dynT->setProperty("baum", 42);
	QTest::newRow("dynamicProperties") << QJsonObject({
														  {"id", 42},
														  {"name", "baum"},
														  {"stateMap", QJsonArray()},
														  {"child", QJsonValue::Null},
														  {"relatives", QJsonArray()},
														  {"baum", 42}
													  })
									   << dynT
									   << false;

	QTest::newRow("invalidDataType") << QJsonObject({
														{"id", -1},
														{"name", QString()},
														{"stateMap", 42},
														{"child", QJsonValue::Null},
														{"relatives", QJsonArray()}
													})
									 << new TestObject(this)
									 << true;

	QTest::newRow("invalidDataType") << QJsonObject({
														{"id", -1},
														{"name", QString()},
														{"stateMap", QJsonArray()},
														{"child", QJsonValue::Null},
														{"relatives", QJsonArray()},
														{"broken", QJsonValue::Null}
													})
									 << (TestObject*)new BrokenTestObject(this)
									 << true;

	QTest::newRow("testImplicitListConvert") << QJsonObject({
																{"id", 42},
																{"name", "baum"},
																{"stateMap", QJsonArray({QJsonValue(true), QJsonValue(42), QJsonValue(0)})},
																{"child", QJsonValue::Null},
																{"relatives", QJsonArray()}
															})
											 << new TestObject(42, "baum", {true, true, false}, -1, this)
											 << false;
}

void RestObjectTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(TestObject*, result);
	QFETCH(bool, shouldFail);

	if(shouldFail)
		QVERIFY_EXCEPTION_THROWN(ser->deserialize(data, result->metaObject(), this), QtRestClient::SerializerException);//to allow broken type
	else {
		auto obj = ser->deserialize(data, &TestObject::staticMetaObject, this);
		QVERIFY(obj);
		auto tObj = qobject_cast<TestObject*>(obj);
		QVERIFY(tObj);

		QVERIFY(result->equals(tObj));

		obj->deleteLater();
	}

	result->deleteLater();
}

QTEST_MAIN(RestObjectTest)

#include "tst_restobject.moc"
