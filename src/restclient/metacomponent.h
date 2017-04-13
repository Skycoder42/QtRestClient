#ifndef QTRESTCLIENT_METACOMPONENT_H
#define QTRESTCLIENT_METACOMPONENT_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qobject.h>

namespace QtRestClient {

//! A helper class to perform operations on either a QObject or a Q_GADGET
template <typename T, typename = void>
class MetaComponent
{
public:
#if !defined(Q_OS_WIN) || defined(__MINGW32__)
	//! Member to test if the meto component has a metaobject
	typedef std::false_type is_meta;
#else //Static assert does not seem to work for windows -> disable it -_-
	typedef std::true_type is_meta;
#endif
};

//! Specialization of MetaComponent for QObject types
template <typename T>
class MetaComponent<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>
{
public:
	//! Member to test if the meto component has a metaobject
	typedef std::true_type is_meta;
	//! calls QObject::deleteLater
	static inline void deleteLater(T *obj) {
		obj->deleteLater();
	}
	//! calls QObject::deleteLater for all list elements
	static inline void deleteAllLater(const QList<T*> &list) {
		foreach(T *obj, list)
			obj->deleteLater();
	}
};

//! Specialization of MetaComponent for Q_GADGET types
template <typename T>
class MetaComponent<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>
{
public:
	//! Member to test if the meto component has a metaobject
	typedef std::true_type is_meta;
	//! does nothing
	static inline void deleteLater(T) {}
	//! does nothing
	static inline void deleteAllLater(const QList<T> &) {}
};

}

#endif // QTRESTCLIENT_METACOMPONENT_H
