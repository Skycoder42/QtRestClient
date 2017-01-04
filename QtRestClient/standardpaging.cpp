#include "jsonserializer.h"
#include "standardpaging.h"
using namespace QtRestClient;

namespace QtRestClient {
class StandardPagingPrivate
{
public:
	StandardPagingPrivate(const QJsonObject &object);

	int total;
	int offset;
	int limit;
	QUrl prev;
	QUrl next;
	QJsonArray items;
};

}

#define d d_ptr

StandardPaging::StandardPaging(const QJsonObject &object) :
	IPaging(),
	d_ptr(new StandardPagingPrivate(object))
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

int StandardPaging::limit() const
{
	return d->limit;
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

// ------------- Factory Implementation -------------

IPaging *StandardPagingFactory::createPaging(const QJsonObject &data) const
{
	if(!data["total"].isDouble() ||
	   !data["offset"].isDouble() ||
	   !data["limit"].isDouble() ||
	   !validateUrl(data["previous"]) ||
	   !validateUrl(data["next"]) ||
	   !data["items"].isArray())
		throw SerializerException("Given JSON is not a default paging object!", true);
	return new StandardPaging(data);
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

StandardPagingPrivate::StandardPagingPrivate(const QJsonObject &object) :
	total(object["total"].toInt()),
	offset(object["offset"].toInt()),
	limit(object["limit"].toInt()),
	prev(object["previous"].isNull() ? QUrl() : object["previous"].toString()),
	next(object["next"].isNull() ? QUrl() : object["next"].toString()),
	items(object["items"].toArray())
{}
