#include <QtJsonSerializer/QJsonSerializer>
#include "standardpaging.h"
using namespace QtRestClient;

namespace QtRestClient {
class StandardPagingPrivate
{
public:
	StandardPagingPrivate();

	int total;
	int offset;
	QUrl prev;
	QUrl next;
	QJsonArray items;
	QJsonObject json;
};

}

#define d d_ptr

StandardPaging::StandardPaging() :
	IPaging(),
	d_ptr(new StandardPagingPrivate())
{}

StandardPaging::~StandardPaging() {}

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

void StandardPaging::setItems(const QJsonArray &items)
{
	d->items = items;
}

void StandardPaging::setTotal(int total)
{
	d->total = total;
}

void StandardPaging::setOffset(int offset)
{
	d->offset = offset;
}

void StandardPaging::setNext(const QUrl &next)
{
	d->next = next;
}

void StandardPaging::setPrevious(const QUrl &previous)
{
	d->prev = previous;
}

void StandardPaging::setJson(const QJsonObject &object)
{
	d->json = object;
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

// ------------- Private Implementation -------------

StandardPagingPrivate::StandardPagingPrivate() :
	total(INT_MAX),
	offset(-1),
	prev(),
	next(),
	items(),
	json()
{}
