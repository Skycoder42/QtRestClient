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

bool TestObject::equals(const TestObject *other) const
{
	if(this == other)
		return true;
	if(!this || !other)
		return false;

	auto base = id == other->id &&
		name == other->name &&
		stateMap == other->stateMap &&
		child->equals(other->child) &&
		relatives.size() == other->relatives.size();
	if(base) {
		for(auto i = 0; i < relatives.size(); i++) {
			if(!relatives[i]->equals(other->relatives[i]))
				return false;
		}

		if(this->dynamicPropertyNames().contains("baum"))
			return this->property("baum") == other->property("baum");
		else
			return true;
	} else
		return false;
}
