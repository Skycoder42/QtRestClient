#ifndef QTRESTCLIENT_METACOMPONENT_H
#define QTRESTCLIENT_METACOMPONENT_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qobject.h>
#include <QtJsonSerializer/qjsonserializer_helpertypes.h>

namespace QtRestClient {

//! @private
template <typename T>
class MetaComponent : public _qjsonserializer_helpertypes::gadget_helper<T>
{
public:
	static inline void deleteLater(T) {}
	static inline void deleteAllLater(const QList<T> &) {}
};

//! @private
template <typename T>
class MetaComponent<T*> : public std::is_base_of<QObject, T>
{
public:
	static inline void deleteLater(T *obj) {
		obj->deleteLater();
	}
	static inline void deleteAllLater(const QList<T*> &list) {
		for(T *obj : list)
			obj->deleteLater();
	}
};

}

#endif // QTRESTCLIENT_METACOMPONENT_H
