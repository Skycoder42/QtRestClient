#ifndef RESTOBJECT_H
#define RESTOBJECT_H

#include "qtrestclient_global.h"

#include <QObject>

namespace QtRestClient {

class QTRESTCLIENTSHARED_EXPORT RestObject : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE RestObject(QObject *parent = nullptr);

	virtual bool equals(const RestObject *other) const;
};

}

Q_DECLARE_METATYPE(QtRestClient::RestObject*)

#define QTRESTCLIENT_OBJECT_LIST_PROPERTY(name, type) \
	Q_PROPERTY(int __qtrc_ro_olp_ ## name READ __qtrc_ro_olp_ ## name STORED false DESIGNABLE false CONSTANT FINAL) \
	inline int __qtrc_ro_olp_ ## name() const { \
		return qMetaTypeId<type>();\
	}

#endif // RESTOBJECT_H
