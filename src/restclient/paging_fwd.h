#ifndef PAGING_FWD_H
#define PAGING_FWD_H

#include "QtRestClient/qtrestclient_global.h"

#include "QtRestClient/ipaging.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/metacomponent.h"

#include <QtJsonSerializer/qjsonserializerexception.h>
#include <QtCore/qshareddata.h>
#include <functional>

namespace QtRestClient {

template<typename DO, typename EO>
class GenericRestReply;

template<typename T>
class PagingData;

//! A class to access generic paging objects
template<typename T>
class Paging
{
	static_assert(MetaComponent<T>::is_meta::value, "T must inherit QObject or have Q_GADGET!");
public:
	//! Default Constructor
	Paging();
	//! Copy Constructor
	Paging(const Paging<T> &other);
	//! Constructs a paging from the interface, the data and a client
	Paging(IPaging *iPaging, const QList<T> &data, RestClient *client);

	//! Returns true, if the current paging object is a valid one
	bool isValid() const;

	//! @copybrief IPaging::items
	QList<T> items() const;

	//! @copybrief IPaging::total
	int total() const;
	//! @copybrief IPaging::offset
	int offset() const;
	//! @copybrief IPaging::limit
	int limit() const;

	//! @copybrief IPaging::hasNext
	bool hasNext() const;
	//! Performs a request for the next paging object
	template<typename EO = QObject*>
	GenericRestReply<Paging<T>, EO> *next() const;
	//! @copybrief IPaging::next
	QUrl nextUrl() const;

	//! @copybrief IPaging::hasPrevious
	bool hasPrevious() const;
	//! Performs a request for the previous paging object
	template<typename EO = QObject*>
	GenericRestReply<Paging<T>, EO> *previous() const;
	//! @copybrief IPaging::previous
	QUrl previousUrl() const;

	//! Iterates over all paging objects
	void iterate(std::function<bool(T, int)> iterator, int to = -1, int from = 0) const;
	//! Iterates over all paging objects, with error handling
	template<typename EO = QObject*>
	void iterate(std::function<bool(T, int)> iterator,
				 std::function<void(QString, int, RestReply::ErrorType)> errorHandler,
				 std::function<QString(EO, int)> failureTransformer = {},
				 int to = -1,
				 int from = 0) const;
	//! Iterates over all paging objects, with error handling
	template<typename EO = QObject*>
	void iterate(std::function<bool(T, int)> iterator,
				 std::function<void(int, EO)> failureHandler,
				 std::function<void(QString, int, RestReply::ErrorType)> errorHandler = {},
				 std::function<void(QJsonSerializerException &)> exceptionHandler = {},
				 int to = -1,
				 int from = 0) const;

	//! Deletes all items this paging object is holding (QObjects only)
	void deleteAllItems() const;

private:
	QSharedDataPointer<PagingData<T>> d;

	int internalIterate(std::function<bool(T, int)> iterator, int from, int to) const;
};

}

#endif // PAGING_FWD_H
