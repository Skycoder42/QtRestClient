#ifndef PAGING_H
#define PAGING_H

#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/genericrestreply.h"
#include <QtCore/qsharedpointer.h>

// ------------- Generic Implementation -------------

namespace QtRestClient {

template <typename T>
class PagingData : public QSharedData
{
public:
	PagingData();
	PagingData(const PagingData &other);

	QSharedPointer<IPaging> iPaging;
	QList<T> data;
	RestClient *client;
};

template<typename T>
Paging<T>::Paging() :
	d(new PagingData<T>())
{}

template<typename T>
Paging<T>::Paging(const Paging<T> &other) :
	d(other.d)
{}

template<typename T>
Paging<T>::Paging(IPaging *iPaging, const QList<T> &data, RestClient *client) :
	d(new PagingData<T>())
{
	d->iPaging.reset(iPaging);
	d->data = data;
	d->client = client;
}

template<typename T>
bool Paging<T>::isValid() const
{
	return d->iPaging;
}

template<typename T>
QList<T> Paging<T>::items() const
{
	return d->data;
}

template<typename T>
int Paging<T>::total() const
{
	return d->iPaging->total();
}

template<typename T>
int Paging<T>::offset() const
{
	return d->iPaging->offset();
}

template<typename T>
int Paging<T>::limit() const
{
	return d->iPaging->limit();
}

template<typename T>
bool Paging<T>::hasNext() const
{
	return d->iPaging->hasNext();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::next() const
{
	if(d->iPaging->hasNext()) {
		auto reply = d->client->builder()
				.updateFromRelativeUrl(d->iPaging->next(), true)
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, d->client, d->client);
	} else
		return nullptr;
}

template<typename T>
QUrl Paging<T>::nextUrl() const
{
	return d->iPaging->next();
}

template<typename T>
bool Paging<T>::hasPrevious() const
{
	return d->iPaging->hasPrevious();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::previous() const
{
	if(d->iPaging->hasPrevious()) {
		auto reply = d->client->builder()
				.updateFromRelativeUrl(d->iPaging->previous(), true)
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, d->client, d->client);
	} else
		return nullptr;
}

template<typename T>
QUrl Paging<T>::previousUrl() const
{
	return d->iPaging->previous();
}

template<typename T>
void Paging<T>::iterate(std::function<bool (T, int)> iterator, int to, int from) const
{
	return iterate(iterator, {}, {}, to, from);
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(std::function<bool(T, int)> iterator, std::function<void(QString, int, RestReply::ErrorType)> errorHandler, std::function<QString (EO, int)> failureTransformer, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//continue to the next one
	int max;
	if(to >= 0)
		max = qMin(to, d->iPaging->total());
	else
		max = d->iPaging->total();
	if(index < max && d->iPaging->hasNext()) {
		next()->enableAutoDelete()
			  ->onSucceeded([=](int, Paging<T> paging) {
				  paging.iterate(iterator, errorHandler, failureTransformer, to, index);
			  })
			  ->onAllErrors(errorHandler, failureTransformer);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(std::function<bool(T, int)> iterator, std::function<void(int, EO)> failureHandler, std::function<void(QString, int, RestReply::ErrorType)> errorHandler, std::function<void(QJsonSerializerException &)> exceptionHandler, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//continue to the next one
	int max;
	if(to >= 0)
		max = qMin(to, d->iPaging->total());
	else
		max = d->iPaging->total();
	if(index < max && d->iPaging->hasNext()) {
		next()->enableAutoDelete()
			  ->onSucceeded([=](int, Paging<T> paging) {
				  paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, index);
			  })
			  ->onFailed(failureHandler)
			  ->onError(errorHandler)
			  ->onSerializeException(exceptionHandler);
	}
}

template<typename T>
void Paging<T>::deleteAllItems() const
{
	MetaComponent<T>::deleteAllLater(d->data);
}

template<typename T>
int Paging<T>::internalIterate(std::function<bool (T, int)> iterator, int to, int from) const
{
	//handle all items in this paging
	int i, max;
	if(to >= 0)
		max = qMin(to, d->iPaging->limit());
	else
		max = d->iPaging->limit();

	auto canceled = false;
	for(i = qMax(from, d->iPaging->offset()); i < max; i++) {
		auto item = d->data.value(i - d->iPaging->offset());
		if(!iterator(item, i)) {
			canceled = true;
			break;
		}
	}

	//delete all unused items
	for(auto j = d->iPaging->offset(); j < from; j++)
		MetaComponent<T>::deleteLater(d->data.value(j - d->iPaging->offset()));
	for(auto j = i+1; j < d->iPaging->limit(); j++)
		MetaComponent<T>::deleteLater(d->data.value(j - d->iPaging->offset()));
	return canceled ? -1 : i;
}

template<typename T>
PagingData<T>::PagingData() :
	QSharedData(),
	iPaging(nullptr),
	data(),
	client(nullptr)
{}

template<typename T>
PagingData<T>::PagingData(const PagingData &other) :
	QSharedData(other),
	iPaging(other.iPaging),
	data(other.data),
	client(other.client)
{}

}

#endif // PAGING_H
