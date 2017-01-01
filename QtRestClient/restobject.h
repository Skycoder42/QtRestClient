#ifndef RESTOBJECT_H
#define RESTOBJECT_H

#include "qtrestclient_global.h"

#include <QObject>

namespace QtRestClient {

class QTRESTCLIENTSHARED_EXPORT RestObject : public QObject
{
	Q_OBJECT

public:
	explicit RestObject(QObject *parent = nullptr);
};

}

Q_DECLARE_METATYPE(QtRestClient::RestObject*)

#define QTRESTCLIENT_OBJECT_LIST_PROPERTY(type, name) \
	Q_PROPERTY(int __qtrc_ro_olp_ ## name READ __qtrc_ro_olp_ ## name STORED false DESIGNABLE false CONSTANT FINAL) \
	inline int __qtrc_ro_olp_ ## name() const { \
		return qMetaTypeId<type>();\
	}

#endif // RESTOBJECT_H
