#ifndef QTRESTCLIENT_METACOMPONENT_H
#define QTRESTCLIENT_METACOMPONENT_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/qtrestclient_helpertypes.h"

#include <QtCore/qobject.h>
#include <QtJsonSerializer/qtjsonserializer_helpertypes.h>

namespace QtRestClient::__private {

template <typename T>
using EnableGadgetType = typename std::enable_if<QtJsonSerializer::__private::gadget_helper<T>::value>::type;

template <typename T>
using EnableObjectType = typename std::enable_if<std::is_base_of<QObject, T>::value>::type;

template <typename T, typename Enable = void>
class MetaComponent : public std::false_type {};

template <typename T>
class MetaComponent<T, EnableGadgetType<T>> : public std::true_type
{
public:
	static inline void deleteLater(T) {}
	static inline void deleteAllLater(const QList<T> &) {}
};

template <typename T>
class MetaComponent<T*, EnableObjectType<T>> : public std::true_type
{
public:
	static inline void deleteLater(T *obj) {
		if (obj)
			obj->deleteLater();
	}
	static inline void deleteAllLater(const QList<T*> &list) {
		for (T *obj : list) {
			if (obj)
				obj->deleteLater();
		}
	}
};

template <typename T>
class MetaComponent<T*, EnableGadgetType<T>> : public std::true_type
{
public:
	static inline void deleteLater(T *gad) {
		delete gad;
	}
	static inline void deleteAllLater(const QList<T*> &list) {
		qDeleteAll(list);
	}
};

}

#endif // QTRESTCLIENT_METACOMPONENT_H
