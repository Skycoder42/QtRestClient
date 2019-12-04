#include "standardpaging_p.h"
#include "metacomponent.h"
using namespace QtRestClient;

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include <QtJsonSerializer/SerializerBase>
using namespace QtJsonSerializer;
#endif

std::variant<QCborArray, QJsonArray> StandardPaging::items() const
{
	return _items;
}

qint64 StandardPaging::total() const
{
	return _total;
}

qint64 StandardPaging::offset() const
{
	return _offset;
}

bool StandardPaging::hasNext() const
{
	return _next.isValid();
}

QUrl StandardPaging::next() const
{
	return _next;
}

bool StandardPaging::hasPrevious() const
{
	return _prev.isValid();
}

QUrl StandardPaging::previous() const
{
	return _prev;
}

QVariantMap StandardPaging::properties() const
{
	return std::visit([](auto data){
			   return data.toVariant();
		   }, _data).toMap();
}

std::variant<QCborValue, QJsonValue> StandardPaging::originalData() const
{
	return _data;
}

// ------------- Factory Implementation -------------

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
IPaging *StandardPagingFactory::createPaging(SerializerBase *, const std::variant<QCborValue, QJsonValue> &data) const
#else
IPaging *StandardPagingFactory::createPaging(const std::variant<QCborValue, QJsonValue> &data) const
#endif
{
	auto paging = new StandardPaging{};
	std::visit(__private::overload {
				   [paging](const QCborValue &value) {
					   const auto map = value.toMap();
					   paging->_items = map[QStringLiteral("items")].toArray();
					   paging->_total = map[QStringLiteral("total")].toInteger(paging->_total);
					   paging->_offset = map[QStringLiteral("offset")].toInteger(paging->_offset);
					   if (const auto url = extractUrl(map[QStringLiteral("next")]); url)
						   paging->_next = *url;
					   if (const auto url = extractUrl(map[QStringLiteral("previous")]); url)
						   paging->_prev = *url;
				   },
				   [paging](const QJsonValue &value) {
					   const auto obj = value.toObject();
					   paging->_items = obj[QStringLiteral("items")].toArray();
					   paging->_total = obj[QStringLiteral("total")].toInt(static_cast<int>(paging->_total));
					   paging->_offset = obj[QStringLiteral("offset")].toInt(static_cast<int>(paging->_offset));
					   if (const auto url = extractUrl(obj[QStringLiteral("next")]); url)
						   paging->_next = *url;
					   if (const auto url = extractUrl(obj[QStringLiteral("previous")]); url)
						   paging->_prev = *url;
				   }
			   }, data);
	paging->_data = data;
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
