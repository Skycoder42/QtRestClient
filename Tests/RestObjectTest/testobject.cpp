#include "testobject.h"

TestObject::TestObject(QObject *parent) :
	RestObject(parent),
	id(-1),
	name(),
	stateMap(),
	child(nullptr)
{}
