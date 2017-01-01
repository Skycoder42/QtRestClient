#include "testobject.h"

TestObject::TestObject(QObject *parent) :
	RestObject(parent),
	id(-1),
	name(),
	stateMap(),
	child(nullptr),
	relatives()
{}

TestObject::TestObject(int id, QString name, QList<bool> stateMap, int childId, QObject *parent) :
	RestObject(parent),
	id(id),
	name(name),
	stateMap(stateMap),
	child(childId > 0 ? new TestObject(childId, {}, {}, -1, this) : nullptr),
	relatives()
{}
