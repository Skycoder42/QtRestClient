#include "standardpaging_p.h"
#include "qtrestclient_helpertypes.h"
using namespace QtRestClient;

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include <QtJsonSerializer/SerializerBase>
using namespace QtJsonSerializer;
#endif

QVariantMap StandardCborPaging::properties() const
{
	return _data.toVariant().toMap();
}

QCborArray StandardCborPaging::cborItems() const
{
	return _items;
}

QCborValue StandardCborPaging::originalCbor() const
{
	return _data;
}

QVariantMap StandardJsonPaging::properties() const
{
	return _data.toVariant().toMap();
}

QJsonArray StandardJsonPaging::jsonItems() const
{
	return _items;
}

QJsonValue StandardJsonPaging::originalJson() const
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
	return std::visit(__private::overload {
						  [](const QCborValue &value) -> IPaging* {
							  const auto map = value.toMap();
							  auto paging = new StandardCborPaging{};
							  paging->_total = map[QStringLiteral("total")].toInteger(paging->_total);
							  paging->_offset = map[QStringLiteral("offset")].toInteger(paging->_offset);
							  if (const auto url = extractUrl(map[QStringLiteral("next")]); url)
								  paging->_next = *url;
							  if (const auto url = extractUrl(map[QStringLiteral("previous")]); url)
								  paging->_prev = *url;
							  paging->_items = map[QStringLiteral("items")].toArray();
							  paging->_data = value;
							  return paging;
						  },
						  [](const QJsonValue &value) -> IPaging* {
							  const auto obj = value.toObject();
							  auto paging = new StandardJsonPaging{};
							  paging->_total = obj[QStringLiteral("total")].toInt(static_cast<int>(paging->_total));
							  paging->_offset = obj[QStringLiteral("offset")].toInt(static_cast<int>(paging->_offset));
							  if (const auto url = extractUrl(obj[QStringLiteral("next")]); url)
								  paging->_next = *url;
							  if (const auto url = extractUrl(obj[QStringLiteral("previous")]); url)
								  paging->_prev = *url;
							  paging->_items = obj[QStringLiteral("items")].toArray();
							  paging->_data = value;
							  return paging;
						  }
					  }, data);
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
