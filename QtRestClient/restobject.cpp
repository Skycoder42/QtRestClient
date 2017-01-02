#include "restobject.h"
#include <QDebug>
#include <QMetaMethod>
using namespace QtRestClient;

RestObject::RestObject(QObject *parent) :
	QObject(parent)
{}

bool RestObject::equals(const RestObject *left, const RestObject *right)
{
	if(left) {
		if(!left->equals(right))
			return false;
	} else if(left != right)
		return false;
	return true;
}

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
				if(!equals(c1, c2))
					return false;
			} else {
				auto listIndex = metaObject()->indexOfProperty(QByteArray("__qtrc_ro_olp_") + property.name());
				if(listIndex != -1) {
					auto vList1 = property.read(this).toList();
					auto vList2 = property.read(other).toList();
					if(vList1.size() != vList2.size())
						return false;
					for(auto i = 0; i < vList1.size(); i++) {
						auto c1 = vList1[i].value<RestObject*>();
						auto c2 = vList2[i].value<RestObject*>();
						if(!equals(c1, c2))
							return false;
					}
				} else if(property.read(this) != property.read(other))
					return false;
			}
		}
		return true;
	}
}
