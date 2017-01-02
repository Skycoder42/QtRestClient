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
	auto ok1 = QMetaType::registerConverter<QList<T>, QVariantList>([](const QList<T> &list) -> QVariantList {
		QVariantList l;
		foreach(auto v, list)
			l.append(QVariant::fromValue(v));
		return l;
	});

	auto ok2 = QMetaType::registerConverter<QVariantList, QList<T>>([](const QVariantList &list) -> QList<T> {
		QList<T> l;
		foreach(auto v, list) {
			auto vt = v.type();
			if(v.convert(qMetaTypeId<T>()))
				l.append(v.value<T>());
			else {
				qWarning() << "Conversion to"
						   << QMetaType::typeName(qMetaTypeId<QList<T>>())
						   << "failed, could not convert element of type"
						   << QMetaType::typeName(vt);
				l.append(T());
			}
		}
		return l;
	});

	auto ok3 = QMetaType::registerComparators<QList<T>>();

	return ok1 && ok2 && ok3;
}

}

#endif // QTRESTCLIENT_GLOBAL_H
