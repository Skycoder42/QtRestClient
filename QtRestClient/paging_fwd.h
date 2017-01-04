#ifndef PAGING_FWD_H
#define PAGING_FWD_H

#include "qtrestclient_global.h"

#include "ipaging.h"
#include "restclient.h"

#include <functional>

namespace QtRestClient {

template<typename DO, typename EO>
class GenericRestReply;

template<typename T>
class Paging
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
public:
	Paging();
	Paging(IPaging *iPaging, const QList<T*> &data, RestClient *client);

	bool isValid() const;

	QList<T*> items() const;

	int total() const;
	int offset() const;
	int limit() const;

	bool hasNext() const;
	template<typename EO = RestObject>
	GenericRestReply<Paging<T>, EO> *next() const;
	QUrl nextUrl() const;

	bool hasPrevious() const;
	template<typename EO = RestObject>
	GenericRestReply<Paging<T>, EO> *previous() const;
	QUrl previousUrl() const;

	void iterate(std::function<bool(Paging<T>*, T*, int)> iterator, int to = -1, int from = 0);
	template<typename EO = RestObject>
	void iterate(std::function<bool(Paging<T>*, T*, int)> iterator,
				 std::function<void(RestReply*, int, EO*)> errorHandler,
				 std::function<void(RestReply*, SerializerException &)> exceptionHandler,
				 int to = -1,
				 int from = 0);

public slots:
	void deleteAllItems();

private:
	IPaging *iPaging;
	QList<T*> data;
	RestClient *client;

	int internalIterate(std::function<bool(Paging<T>*, T*, int)> iterator, int from, int to);
};

}

#endif // PAGING_FWD_H
