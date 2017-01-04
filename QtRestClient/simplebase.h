#ifndef SIMPLEBASE_H
#define SIMPLEBASE_H

#include "qtrestclient_global.h"

#include "genericrestreply.h"

namespace QtRestClient {

class QTRESTCLIENTSHARED_EXPORT SimpleTypeBase : public RestObject
{
public:
	virtual QUrl href() const = 0;
};

template<typename T>
class SimpleType : public SimpleTypeBase
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
public:

	T *currentExtended() const;

	template<typename ET = RestObject>
	GenericRestReply<T, ET> extend(bool takeOwn = false);

private:
	T *cExt;
};

}

#endif // SIMPLEBASE_H
