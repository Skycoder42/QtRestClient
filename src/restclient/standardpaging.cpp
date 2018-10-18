#include <limits>
#include <QtJsonSerializer/QJsonSerializer>
#include "standardpaging_p.h"
using namespace QtRestClient;

namespace QtRestClient {

class StandardPagingPrivate
{
public:
	int total = std::numeric_limits<int>::max();
	int offset = -1;
	QUrl prev;
	QUrl next;
	QJsonArray items;
	QJsonObject json;
};

}

StandardPaging::StandardPaging() :
	IPaging(),
	d(new StandardPagingPrivate())
{}

StandardPaging::StandardPaging(const StandardPaging &other) = default;

StandardPaging::StandardPaging(StandardPaging &&other) noexcept = default;

StandardPaging &StandardPaging::operator=(const StandardPaging &other) = default;

StandardPaging &StandardPaging::operator=(StandardPaging &&other) noexcept = default;

StandardPaging::~StandardPaging() = default;

QJsonArray StandardPaging::items() const
{
	return d->items;
}

int StandardPaging::total() const
{
	return d->total;
}

int StandardPaging::offset() const
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
	return QJsonValue(d->json).toVariant().toMap();
}

QJsonObject StandardPaging::originalJson() const
{
	return d->json;
}

void StandardPaging::setItems(QJsonArray items)
{
	d->items = std::move(items);
}

void StandardPaging::setTotal(int total)
{
	d->total = total;
}

void StandardPaging::setOffset(int offset)
{
	d->offset = offset;
}

void StandardPaging::setNext(QUrl next)
{
	d->next = std::move(next);
}

void StandardPaging::setPrevious(QUrl previous)
{
	d->prev = std::move(previous);
}

void StandardPaging::setJson(QJsonObject object)
{
	d->json = std::move(object);
}

// ------------- Factory Implementation -------------

IPaging *StandardPagingFactory::createPaging(QJsonSerializer *serializer, const QJsonObject &data) const
{
	//validate data and next only -> only ones required
	if(!validateUrl(data[QStringLiteral("next")]) ||
	   !data[QStringLiteral("items")].isArray())
		throw QJsonDeserializationException("Given JSON is not a default paging object!");
	auto paging = new StandardPaging(serializer->deserialize<StandardPaging>(data));
	paging->setJson(data);
	return paging;
}

bool StandardPagingFactory::validateUrl(const QJsonValue &value)
{
	if(value.isNull())
		return true;
	else if(value.isString())
		return QUrl(value.toString()).isValid();
	else
		return false;
}
