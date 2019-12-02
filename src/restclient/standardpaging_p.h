#ifndef QTRESTCLIENT_STANDARDPAGING_P_H
#define QTRESTCLIENT_STANDARDPAGING_P_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <optional>

#include <QtCore/qsharedpointer.h>

namespace QtRestClient {

class StandardPagingPrivate;
class Q_RESTCLIENT_EXPORT StandardPaging : public IPaging // clazy:exclude=copyable-polymorphic
{
	friend class StandardPagingFactory;

public:
	//! Creates a standard paging from a valid json object
	StandardPaging();
	StandardPaging(const StandardPaging &other);
	StandardPaging(StandardPaging &&other) noexcept;
	StandardPaging &operator=(const StandardPaging &other);
	StandardPaging &operator=(StandardPaging &&other) noexcept;
	~StandardPaging() override;

	std::variant<QCborArray, QJsonArray> items() const override;
	qint64 total() const override;
	qint64 offset() const override;
	bool hasNext() const override;
	QUrl next() const override;
	bool hasPrevious() const override;
	QUrl previous() const override;
	QVariantMap properties() const override;
	std::variant<QCborValue, QJsonValue> originalData() const override;

private:
	QSharedPointer<StandardPagingPrivate> d;
};

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public IPagingFactory
{
public:
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	IPaging *createPaging(QtJsonSerializer::JsonSerializer *serializer, const std::variant<QCborValue, QJsonValue> &data) const override;
#else
	IPaging *createPaging(const std::variant<QCborValue, QJsonValue> &data) const override;
#endif

private:
	static std::optional<QUrl> extractUrl(const std::variant<QCborValue, QJsonValue> &value);
};

}

#endif // QTRESTCLIENT_STANDARDPAGING_P_H
