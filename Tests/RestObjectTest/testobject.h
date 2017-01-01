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

public:
	Q_INVOKABLE TestObject(QObject *parent = nullptr);

	int id;
	QString name;
	QList<bool> stateMap;
	TestObject* child;
};

#endif // TESTOBJECT_H
