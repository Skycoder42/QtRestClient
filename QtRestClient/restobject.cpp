#include "restobject.h"
#include <QDebug>
#include <QMetaMethod>
using namespace QtRestClient;

RestObject::RestObject(QObject *parent) :
	QObject(parent)
{}

bool RestObject::equals(const RestObject *other) const
{
	if(this == other)
		return true;
	else if(!other)
		return false;
	else if(metaObject()->className() != other->metaObject()->className())
		return false;
	else {
		for(auto i = staticMetaObject.propertyOffset(); i < metaObject()->propertyCount(); i++) {
			auto property = metaObject()->property(i);
			QMetaType t(property.userType());
			if(t.flags().testFlag(QMetaType::PointerToQObject) &&
			   t.metaObject()->inherits(&staticMetaObject)) {
				auto c1 = property.read(this).value<RestObject*>();
				auto c2 = property.read(other).value<RestObject*>();
				if(c1) {
					if(!c1->equals(c2))
						return false;
				} else if(c1 != c2)
					return false;
			} else if(property.read(this) != property.read(other))
				return false;
			//TODO custom comparator for list of restObject
		}
		return true;
	}
}
