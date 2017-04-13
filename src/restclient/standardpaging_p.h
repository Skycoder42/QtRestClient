#ifndef QTRESTCLIENT_STANDARDPAGING_P_H
#define QTRESTCLIENT_STANDARDPAGING_P_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <QtCore/qsharedpointer.h>

namespace QtRestClient {

class StandardPagingPrivate;
class Q_RESTCLIENT_EXPORT StandardPaging : public IPaging
{
	Q_GADGET

	Q_PROPERTY(QJsonArray items READ items WRITE setItems)
	Q_PROPERTY(int total READ total WRITE setTotal)
	Q_PROPERTY(int offset READ offset WRITE setOffset)
	Q_PROPERTY(QUrl next READ next WRITE setNext)
	Q_PROPERTY(QUrl previous READ previous WRITE setPrevious)

	friend class StandardPagingFactory;

public:
	//! Creates a standard paging from a valid json object
	StandardPaging();
	~StandardPaging();

	QJsonArray items() const override;
	int total() const override;
	int offset() const override;
	bool hasNext() const override;
	QUrl next() const override;
	bool hasPrevious() const override;
	QUrl previous() const override;
	QVariantMap properties() const override;
	QJsonObject originalJson() const override;

private:
	QSharedPointer<StandardPagingPrivate> d;

	void setItems(const QJsonArray &items);
	void setTotal(int total);
	void setOffset(int offset);
	void setNext(const QUrl &next);
	void setPrevious(const QUrl &previous);
	void setJson(const QJsonObject &object);
};

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public PagingFactory
{
public:
	IPaging *createPaging(QJsonSerializer *serializer, const QJsonObject &data) const override;

private:
	static bool validateUrl(const QJsonValue &value);
};

}

#endif // QTRESTCLIENT_STANDARDPAGING_P_H
