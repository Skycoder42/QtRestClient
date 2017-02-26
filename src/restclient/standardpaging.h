#ifndef STANDARDPAGING_H
#define STANDARDPAGING_H

#include "qtrestclient_global.h"

#include "ipaging.h"

namespace QtRestClient {

class StandardPagingPrivate;
class QTRESTCLIENTSHARED_EXPORT StandardPaging : public IPaging
{
public:
	StandardPaging(const QJsonObject &object);
	~StandardPaging();

	QJsonArray items() const override;
	int total() const override;
	int offset() const override;
	int limit() const override;
	bool hasNext() const override;
	QUrl next() const override;
	bool hasPrevious() const override;
	QUrl previous() const override;

private:
	QScopedPointer<StandardPagingPrivate> d_ptr;
};

class QTRESTCLIENTSHARED_EXPORT StandardPagingFactory : public PagingFactory
{
public:
	IPaging *createPaging(const QJsonObject &data) const override;

private:
	static bool validateUrl(const QJsonValue &value);
};

}

#endif // STANDARDPAGING_H
