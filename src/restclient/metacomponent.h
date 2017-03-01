#ifndef METACOMPONENT_H
#define METACOMPONENT_H

#include "QtRestClient/qrestclient_global.h"

#include <QtCore/qobject.h>

namespace QtRestClient {

template <typename T, typename = void>
class MetaComponent
{
public:
	typedef std::false_type is_meta;
};

template <typename T>
class MetaComponent<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>
{
public:
	typedef std::true_type is_meta;
	static inline void deleteLater(T *obj) {
		obj->deleteLater();
	}
};

template <typename T>
class MetaComponent<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>
{
public:
	typedef std::true_type is_meta;
	static inline void deleteLater(T) {}
};

}

#endif // METACOMPONENT_H
