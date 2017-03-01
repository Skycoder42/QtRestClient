#ifndef METADELETE_H
#define METADELETE_H

#include "QtRestClient/qrestclient_global.h"

#include <QtCore/qobject.h>

namespace QtRestClient {

template <typename T, typename = void>
class MetaDelete {};

template <typename T>
class MetaDelete<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type> {
public:
	static inline void deleteLater(T *obj) {
		obj->deleteLater();
	}
};

template <typename T>
class MetaDelete<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type> {
public:
	static inline void deleteLater(T) {}
};

}

#endif // METADELETE_H
