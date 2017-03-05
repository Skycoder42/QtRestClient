#include <QtJsonSerializer/QJsonSerializer>
#include "standardpaging.h"
using namespace QtRestClient;

namespace QtRestClient {
class StandardPagingPrivate
{
public:
	StandardPagingPrivate(const QJsonObject &object);

	int total;
	int offset;
	QUrl prev;
	QUrl next;
	QJsonArray items;
	QVariantMap properties;
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
	return d->properties;
}

// ------------- Factory Implementation -------------

IPaging *StandardPagingFactory::createPaging(QJsonSerializer *, const QJsonObject &data) const
{
	//validate data and next only -> only ones required
	if(!validateUrl(data[QLatin1String("next")]) ||
	   !data[QLatin1String("items")].isArray())
		throw QJsonDeserializationException("Given JSON is not a default paging object!");
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
	total(object[QLatin1String("total")].toInt(INT_MAX)),
	offset(object[QLatin1String("offset")].toInt(-1)),
	prev(object[QLatin1String("previous")].isNull() ? QUrl() : object[QLatin1String("previous")].toString()),
	next(object[QLatin1String("next")].isNull() ? QUrl() : object[QLatin1String("next")].toString()),
	items(object[QLatin1String("items")].toArray()),
	properties(QJsonValue(object).toVariant().toMap())
{}
