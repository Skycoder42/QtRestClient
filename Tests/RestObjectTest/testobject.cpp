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
	if(!RestObject::equals(other))
		return false;
	else if(this && other){
		if(relatives.size() == other->relatives.size()) {
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
	} else
		return false;
}
