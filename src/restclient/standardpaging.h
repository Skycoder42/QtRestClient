#ifndef STANDARDPAGING_H
#define STANDARDPAGING_H

#include "QtRestClient/qrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <QtCore/qscopedpointer.h>

namespace QtRestClient {

class StandardPagingPrivate;
class Q_RESTCLIENT_EXPORT StandardPaging : public IPaging
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

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public PagingFactory
{
public:
	IPaging *createPaging(QJsonSerializer *serializer, const QJsonObject &data) const override;

private:
	static bool validateUrl(const QJsonValue &value);
};

}

#endif // STANDARDPAGING_H
