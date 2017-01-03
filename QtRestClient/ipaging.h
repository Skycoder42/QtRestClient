#ifndef IPAGING_H
#define IPAGING_H

#include "qtrestclient_global.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>

namespace QtRestClient {

class QTRESTCLIENTSHARED_EXPORT IPaging
{
public:
	virtual inline ~IPaging() {}

	virtual QJsonArray items() = 0;

	virtual int total() = 0;
	virtual int offset() = 0;
	virtual int limit() = 0;
	virtual bool hasNext() = 0;
	virtual QUrl next() = 0;
	virtual bool hasPrevious() = 0;
	virtual QUrl previous() = 0;
};

class QTRESTCLIENTSHARED_EXPORT PagingFactory
{
public:
	virtual inline ~PagingFactory() {}

	virtual IPaging *createPaging(const QJsonObject &data) = 0;
};

}

#endif // IPAGING_H
