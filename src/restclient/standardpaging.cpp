#include <limits>

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include <QtJsonSerializer/JsonSerializer>
#endif

#include "standardpaging_p.h"
#include "metacomponent.h"
using namespace QtRestClient;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
using namespace QtJsonSerializer;
#endif

namespace QtRestClient {

class StandardPagingPrivate
{
public:
	qint64 total = std::numeric_limits<qint64>::max();
	qint64 offset = -1;
	QUrl prev;
	QUrl next;
	std::variant<QCborArray, QJsonArray> items;
	std::variant<QCborValue, QJsonValue> data;
};

}

StandardPaging::StandardPaging() :
	  IPaging{},
	  d{new StandardPagingPrivate{}}
{}

StandardPaging::StandardPaging(const StandardPaging &other) = default;

StandardPaging::StandardPaging(StandardPaging &&other) noexcept = default;

StandardPaging &StandardPaging::operator=(const StandardPaging &other) = default;

StandardPaging &StandardPaging::operator=(StandardPaging &&other) noexcept = default;

StandardPaging::~StandardPaging() = default;

std::variant<QCborArray, QJsonArray> StandardPaging::items() const
{
	return d->items;
}

qint64 StandardPaging::total() const
{
	return d->total;
}

qint64 StandardPaging::offset() const
{
	return d->offset;
}

bool StandardPaging::hasNext() const
{
	return d->next.isValid();
}

QUrl StandardPaging::next() const
{
	return d->next;
}

bool StandardPaging::hasPrevious() const
{
	return d->prev.isValid();
}

QUrl StandardPaging::previous() const
{
	return d->prev;
}

QVariantMap StandardPaging::properties() const
{
	return std::visit([](auto data){
			   return data.toVariant();
		   }, d->data).toMap();
}

 std::variant<QCborValue, QJsonValue> StandardPaging::originalData() const
{
	return d->data;
}

// ------------- Factory Implementation -------------

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
IPaging *StandardPagingFactory::createPaging(JsonSerializer *, const std::variant<QCborValue, QJsonValue> &data) const
#else
IPaging *StandardPagingFactory::createPaging(const std::variant<QCborValue, QJsonValue> &data) const
#endif
{
	auto paging = new StandardPaging{};
	std::visit(__private::overload {
				   [paging](const QCborValue &value) {
					   const auto map = value.toMap();
					   paging->d->items = map[QStringLiteral("items")].toArray();
					   paging->d->total = map[QStringLiteral("total")].toInteger();
					   paging->d->offset = map[QStringLiteral("offset")].toInteger();
					   if (const auto url = extractUrl(map[QStringLiteral("next")]); url)
						   paging->d->next = *url;
					   if (const auto url = extractUrl(map[QStringLiteral("previous")]); url)
						   paging->d->prev = *url;
				   },
				   [paging](const QJsonValue &value) {
					   const auto obj = value.toObject();
					   paging->d->items = obj[QStringLiteral("items")].toArray();
					   paging->d->total = obj[QStringLiteral("total")].toInt();
					   paging->d->offset = obj[QStringLiteral("offset")].toInt();
					   if (const auto url = extractUrl(obj[QStringLiteral("next")]); url)
						   paging->d->next = *url;
					   if (const auto url = extractUrl(obj[QStringLiteral("previous")]); url)
						   paging->d->prev = *url;
				   }
			   }, data);
	paging->d->data = data;
	return paging;
}

std::optional<QUrl> StandardPagingFactory::extractUrl(const std::variant<QCborValue, QJsonValue> &value)
{
	return std::visit(__private::overload {
						  [](const QCborValue &data) -> std::optional<QUrl> {
							  if (data.isUrl())
								  return data.toUrl();
							  else
								  return std::nullopt;
						  },
						  [](const QJsonValue &data) -> std::optional<QUrl> {
							  if (data.isString()) {
								  QUrl url{data.toString()};
								  if (url.isValid())
									  return url;
								  else
									  return std::nullopt;
							  } else
								  return std::nullopt;
						  }
					  }, value);
}
