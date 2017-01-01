#ifndef QTRESTCLIENT_GLOBAL_H
#define QTRESTCLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#include <QVariantList>

#if defined(QTRESTCLIENT_LIBRARY)
#  define QTRESTCLIENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTRESTCLIENTSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace QtRestClient {

typedef QHash<QByteArray, QByteArray> HeaderHash;

template<typename T>
static inline bool registerListConverters() {
	auto ok1 = QMetaType::registerConverter<QList<T>, QVariantList>([](const QList<T> &list) -> QVariantList{
		QVariantList l;
		foreach(auto v, list)
			l.append(QVariant::fromValue(v));
		return l;
	});

	auto ok2 = QMetaType::registerConverter<QVariantList, QList<T>>([](const QVariantList &list) -> QList<T> {
		QList<T> l;
		foreach(auto v, list)
			l.append(v.value<T>());
		return l;
	});

	return ok1 && ok2;
}

}

#endif // QTRESTCLIENT_GLOBAL_H
