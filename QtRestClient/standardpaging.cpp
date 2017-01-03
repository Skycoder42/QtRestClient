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
{
	//TODO validate object!
}

QJsonArray StandardPaging::items()
{
	return d->items;
}

int StandardPaging::total()
{
	return d->total;
}

int StandardPaging::offset()
{
	return d->offset;
}

int StandardPaging::limit()
{
	return d->limit;
}

bool StandardPaging::hasNext()
{
	return d->next.isValid();
}

QUrl StandardPaging::next()
{
	return d->next;
}

bool StandardPaging::hasPrevious()
{
	return d->prev.isValid();
}

QUrl StandardPaging::previous()
{
	return d->prev;
}

// ------------- Private Implementation -------------

StandardPagingPrivate::StandardPagingPrivate(const QJsonObject &object) :
	total(object["total"].toInt()),
	offset(object["offset"].toInt()),
	limit(object["limit"].toInt()),
	prev(object["previous"].toString()),
	next(object["next"].toString()),
	items(object["items"].toArray())
{}
