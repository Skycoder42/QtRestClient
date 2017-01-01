#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QtRestClient>

class TestObject : public QtRestClient::RestObject
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QString name MEMBER name)
	Q_PROPERTY(QList<bool> stateMap MEMBER stateMap)
	Q_PROPERTY(TestObject* child MEMBER child)

	Q_PROPERTY(QList<TestObject*> relatives MEMBER relatives)
	QTRESTCLIENT_OBJECT_LIST_PROPERTY(TestObject*, relatives)

public:
	Q_INVOKABLE TestObject(QObject *parent = nullptr);
	TestObject(int id, QString name, QList<bool> stateMap, int childId, QObject *parent = nullptr);

	int id;
	QString name;
	QList<bool> stateMap;
	TestObject* child;
	QList<TestObject*> relatives;
};

#endif // TESTOBJECT_H
