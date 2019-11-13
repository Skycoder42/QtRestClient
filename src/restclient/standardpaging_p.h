#ifndef QTRESTCLIENT_STANDARDPAGING_P_H
#define QTRESTCLIENT_STANDARDPAGING_P_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <QtCore/qsharedpointer.h>

namespace QtRestClient {

class StandardPagingPrivate;
class Q_RESTCLIENT_EXPORT StandardPaging : public IPaging // clazy:exclude=copyable-polymorphic
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
	StandardPaging(const StandardPaging &other);
	StandardPaging(StandardPaging &&other) noexcept;
	StandardPaging &operator=(const StandardPaging &other);
	StandardPaging &operator=(StandardPaging &&other) noexcept;
	~StandardPaging() override;

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

	void setItems(QJsonArray items);
	void setTotal(int total);
	void setOffset(int offset);
	void setNext(QUrl next);
	void setPrevious(QUrl previous);
	void setJson(QJsonObject object);
};

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public IPagingFactory
{
public:
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	IPaging *createPaging(QtJsonSerializer::JsonSerializer *serializer, const QJsonObject &data) const override;
#else
	IPaging *createPaging(const QJsonObject &data) const override;
#endif

private:
	static bool validateUrl(const QJsonValue &value);
};

}

#endif // QTRESTCLIENT_STANDARDPAGING_P_H
