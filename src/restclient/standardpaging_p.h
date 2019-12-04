#ifndef QTRESTCLIENT_STANDARDPAGING_P_H
#define QTRESTCLIENT_STANDARDPAGING_P_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <optional>
#include <limits>

#include <QtCore/QSharedDataPointer>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT StandardPaging : public IPaging // clazy:exclude=copyable-polymorphic
{
	friend class StandardPagingFactory;
public:
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
	qint64 _total = std::numeric_limits<qint64>::max();
	qint64 _offset = -1;
	QUrl _prev;
	QUrl _next;
	std::variant<QCborArray, QJsonArray> _items;
	std::variant<QCborValue, QJsonValue> _data;
};

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public IPagingFactory
{
public:
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	IPaging *createPaging(QtJsonSerializer::SerializerBase *serializer, const std::variant<QCborValue, QJsonValue> &data) const override;
#else
	IPaging *createPaging(const std::variant<QCborValue, QJsonValue> &data) const override;
#endif

private:
	static std::optional<QUrl> extractUrl(const std::variant<QCborValue, QJsonValue> &value);
};

}

#endif // QTRESTCLIENT_STANDARDPAGING_P_H
