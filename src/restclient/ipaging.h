#ifndef IPAGING_H
#define IPAGING_H

#include "QtRestClient/qrestclient_global.h"

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qurl.h>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT IPaging
{
public:
	virtual inline ~IPaging() {}

	virtual QJsonArray items() const = 0;

	virtual int total() const = 0;
	virtual int offset() const = 0;
	virtual int limit() const = 0;
	virtual bool hasNext() const = 0;
	virtual QUrl next() const = 0;
	virtual bool hasPrevious() const = 0;
	virtual QUrl previous() const = 0;
};

class Q_RESTCLIENT_EXPORT PagingFactory
{
public:
	virtual inline ~PagingFactory() {}

	virtual IPaging *createPaging(const QJsonObject &data) const = 0;
};

}

#endif // IPAGING_H
