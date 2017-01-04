#ifndef PAGING_H
#define PAGING_H

#include "qtrestclient_global.h"

#include "ipaging.h"
#include "restclient.h"
#include "genericrestreply.h"

#include <functional>

namespace QtRestClient {

template<typename T>
class QTRESTCLIENTSHARED_EXPORT Paging
{
	//TODO add autodelete like in generic reply
	//TODO add delete items --> needs something like shared data!
public:
	Paging(IPaging *iPaging, const QList<T*> &data, RestClient *client);

	QList<T*> items() const;

	int total() const;
	int offset() const;
	int limit() const;

	bool hasNext() const;
	GenericRestReply<Paging<T>> next() const;
	QUrl nextUrl() const;

	bool hasPrevious() const;
	GenericRestReply<Paging<T>> previous() const;
	QUrl previousUrl() const;

	Paging<T> &iterate(std::function<bool(Paging<T>*, T*, int)> iterator, int from = 0, int to = -1);

private:
	IPaging *iPaging;
	QList<T*> data;
	RestClient *client;
};

}

#endif // PAGING_H
