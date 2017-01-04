#ifndef PAGING_H
#define PAGING_H

#include "paging_fwd.h"
#include "genericrestreply.h"

// ------------- Generic Implementation -------------



template<typename T>
Paging<T>::Paging() :
	iPaging(nullptr),
	data(),
	client(nullptr)
{}

template<typename T>
Paging<T>::Paging(IPaging *iPaging, const QList<T*> &data, RestClient *client) :
	iPaging(iPaging),
	data(data),
	client(client)
{}

template<typename T>
bool Paging<T>::isValid() const
{
	return iPaging;
}

template<typename T>
QList<T*> Paging<T>::items() const
{
	return data;
}

template<typename T>
int Paging<T>::total() const
{
	return iPaging->total();
}

template<typename T>
int Paging<T>::offset() const
{
	return iPaging->offset();
}

template<typename T>
int Paging<T>::limit() const
{
	return iPaging->limit();
}

template<typename T>
bool Paging<T>::hasNext() const
{
	return iPaging->hasNext();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::next() const
{
	if(iPaging->hasNext()) {
		auto reply = client->builder()
				.updateFromRelativeUrl(iPaging->next())
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, client->serializer(), client);
	} else
		return {};
}

template<typename T>
QUrl Paging<T>::nextUrl() const
{
	return iPaging->next();
}

template<typename T>
bool Paging<T>::hasPrevious() const
{
	return iPaging->hasPrevious();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::previous() const
{
	if(iPaging->hasPrevious()) {
		auto reply = client->builder()
				.updateFromRelativeUrl(iPaging->previous())
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, client->serializer(), client);
	} else
		return {};
}

template<typename T>
QUrl Paging<T>::previousUrl() const
{
	return iPaging->previous();
}

template<typename T>
void Paging<T>::iterate(std::function<bool (Paging<T> *, T *, int)> iterator, int to, int from)
{
	return iterate(iterator, {}, {}, to, from);
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(std::function<bool(Paging<T>*, T*, int)> iterator, std::function<void(RestReply *, int, EO*)> errorHandler, std::function<void(RestReply *, SerializerException &)> exceptionHandler, int to, int from)
{
	Q_ASSERT(from >= iPaging->offset());

	auto index = internalIterate(iterator, from ,to);
	if(index < 0)
		return;

	//continue to the next one
	int max;
	if(to >= 0)
		max = qMin(to, iPaging->total());
	else
		max = iPaging->total();
	if(index < max && iPaging->hasNext()) {
		next()->enableAutoDelete()
			  ->onFailed(errorHandler)
			  ->onSerializeException(exceptionHandler)
			  ->onSucceeded([=](RestReply *, int, Paging<T> paging) {
			paging.iterate(iterator, index, to);
		});
	}
}

template<typename T>
void Paging<T>::deleteAllItems()
{
	foreach(auto obj, data)
		obj->deleteLater();
}

template<typename T>
int Paging<T>::internalIterate(std::function<bool (Paging<T>*, T*, int)> iterator, int from, int to)
{
	//handle all items in this paging
	int i, max;
	if(to >= 0)
		max = qMin(to, iPaging->limit());
	else
		max = iPaging->limit();

	auto canceled = false;
	for(i = qMax(from, iPaging->offset()); i < max; i++) {
		auto item = data.value(i - iPaging->offset());
		if(!iterator(this, item, i)) {
			canceled = true;
			break;
		}
	}

	//delete all unused items
	for(auto j = iPaging->offset(); j < from; j++)
		data.value(j - iPaging->offset())->deleteLater();
	for(auto j = i+1; j < iPaging->limit(); j++)
		data.value(j - iPaging->offset())->deleteLater();
	return canceled ? -1 : i;
}

#endif // PAGING_H
