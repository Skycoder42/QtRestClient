#ifndef QTRESTCLIENT_METACOMPONENT_H
#define QTRESTCLIENT_METACOMPONENT_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qobject.h>

namespace QtRestClient {

//! @private
template <typename T, typename = void>
class MetaComponent
{
public:
#if !defined(Q_OS_WIN) || defined(__MINGW32__)
	typedef std::false_type is_meta;
#endif
};

//! @private
template <typename T>
class MetaComponent<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>
{
public:
	typedef std::true_type is_meta;
	static inline void deleteLater(T *obj) {
		obj->deleteLater();
	}
	static inline void deleteAllLater(const QList<T*> &list) {
		for(T *obj : list)
			obj->deleteLater();
	}
};

//! @private
template <typename T>
class MetaComponent<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>
{
public:
	typedef std::true_type is_meta;
	static inline void deleteLater(T) {}
	static inline void deleteAllLater(const QList<T> &) {}
};

}

#endif // QTRESTCLIENT_METACOMPONENT_H
