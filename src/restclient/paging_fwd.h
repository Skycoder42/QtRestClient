#ifndef PAGING_FWD_H
#define PAGING_FWD_H

#include "QtRestClient/qrestclient_global.h"

#include "QtRestClient/ipaging.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/metacomponent.h"

#include <QtJsonSerializer/qjsonserializerexception.h>
#include <functional>

namespace QtRestClient {

template<typename DO, typename EO>
class GenericRestReply;

template<typename T>
class Paging
{
	static_assert(MetaComponent<T>::is_meta::value, "T must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<T>::is_meta::value, "T must inherit QObject or have Q_GADGET!");
public:
	Paging();
	Paging(IPaging *iPaging, const QList<T> &data, RestClient *client);

	bool isValid() const;

	QList<T> items() const;

	int total() const;
	int offset() const;
	int limit() const;

	bool hasNext() const;
	template<typename EO = QObject*>
	GenericRestReply<Paging<T>, EO> *next() const;
	QUrl nextUrl() const;

	bool hasPrevious() const;
	template<typename EO = QObject*>
	GenericRestReply<Paging<T>, EO> *previous() const;
	QUrl previousUrl() const;

	void iterate(std::function<bool(Paging<T>*, T, int)> iterator, int to = -1, int from = 0);
	template<typename EO = QObject*>
	void iterate(std::function<bool(Paging<T>*, T, int)> iterator,
				 std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler,
				 std::function<QString(EO, int)> failureTransformer = {},
				 int to = -1,
				 int from = 0);
	template<typename EO = QObject*>
	void iterate(std::function<bool(Paging<T>*, T, int)> iterator,
				 std::function<void(RestReply*, int, EO)> failureHandler,
				 std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler = {},
				 std::function<void(RestReply*, QJsonSerializerException &)> exceptionHandler = {},
				 int to = -1,
				 int from = 0);

	void deleteAllItems();

private:
	IPaging *iPaging;
	QList<T> data;
	RestClient *client;

	int internalIterate(std::function<bool(Paging<T>*, T, int)> iterator, int from, int to);
};

}

#endif // PAGING_FWD_H
