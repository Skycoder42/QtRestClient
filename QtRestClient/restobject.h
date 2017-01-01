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

#endif // RESTOBJECT_H
