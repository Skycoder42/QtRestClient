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

	QJsonArray items() override;
	int total() override;
	int offset() override;
	int limit() override;
	bool hasNext() override;
	QUrl next() override;
	bool hasPrevious() override;
	QUrl previous() override;

private:
	QScopedPointer<StandardPagingPrivate> d_ptr;
};

}

#endif // STANDARDPAGING_H
