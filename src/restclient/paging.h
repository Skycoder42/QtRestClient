#ifndef QTRESTCLIENT_PAGING_H
#define QTRESTCLIENT_PAGING_H

#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/genericrestreply.h"
#include "QtRestClient/restclass.h"

#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>

// ------------- Generic Implementation -------------

namespace QtRestClient {

namespace __private {

template <typename T>
class PagingData : public QSharedData
{
public:
	PagingData() = default;
	PagingData(const PagingData &other) = default;

	QSharedPointer<IPaging> iPaging;
	QList<T> data;
	QPointer<RestClient> client;
};

}

template<typename T>
Paging<T>::Paging() :
	d{new __private::PagingData<T>{}}
{}

template<typename T>
Paging<T>::Paging(const Paging<T> &other) = default;

template<typename T>
Paging<T>::Paging(Paging<T> &&other) noexcept = default;

template<typename T>
Paging<T> &Paging<T>::operator=(const Paging<T> &other) = default;

template<typename T>
Paging<T> &Paging<T>::operator=(Paging<T> &&other) noexcept = default;

template<typename T>
Paging<T>::Paging(IPaging *iPaging, const QList<T> &data, RestClient *client) :
	d{new __private::PagingData<T>{}}
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
IPaging *Paging<T>::iPaging() const
{
	return d->iPaging.data();
}

template<typename T>
QList<T> Paging<T>::items() const
{
	return d->data;
}

template<typename T>
qint64 Paging<T>::total() const
{
	return d->iPaging->total();
}

template<typename T>
qint64 Paging<T>::offset() const
{
	return d->iPaging->offset();
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
	if (d->iPaging->hasNext())
		return d->client->rootClass()->template get<Paging<T>, EO>(d->iPaging->next());
	else
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
	if (d->iPaging->hasPrevious())
		return d->client->rootClass()->template get<Paging<T>, EO>(d->iPaging->previous());
	else
		return nullptr;
}

template<typename T>
QUrl Paging<T>::previousUrl() const
{
	return d->iPaging->previous();
}

template<typename T>
void Paging<T>::iterate(const std::function<bool (T, qint64)> &iterator, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if (index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index), 
				d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next()->onSucceeded([iterator, to, index](int, const Paging<T> &paging) {
			if (paging.isValid())
				paging.iterate(iterator, to, index);
		});
	}
}

template<typename T>
void Paging<T>::iterate(QObject *scope, const std::function<bool (T, qint64)> &iterator, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if (index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index),
						   d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next()->onSucceeded(scope, [scope, iterator, to, index](int, const Paging<T> &paging) {
			if (paging.isValid())
				paging.iterate(scope, iterator, to, index);
		});
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(const std::function<bool(T, qint64)> &iterator, const std::function<void(QString, int, RestReply::Error)> &errorHandler, const std::function<QString (EO, int)> &failureTransformer, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if (index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index),
						   d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next<EO>()->onSucceeded([iterator, errorHandler, failureTransformer, to, index](int, const Paging<T> &paging) {
					  if (paging.isValid())
						  paging.iterate(iterator, errorHandler, failureTransformer, to, index);
				  })
			->onAllErrors(errorHandler, failureTransformer);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(QObject *scope, const std::function<bool(T, qint64)> &iterator, const std::function<void(QString, int, RestReply::Error)> &errorHandler, const std::function<QString (EO, int)> &failureTransformer, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if (index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index),
						   d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next<EO>()->onSucceeded(scope, [scope, iterator, errorHandler, failureTransformer, to, index](int, const Paging<T> &paging) {
					  if (paging.isValid())
						  paging.iterate(scope, iterator, errorHandler, failureTransformer, to, index);
				  })
			->onAllErrors(scope, errorHandler, failureTransformer);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(const std::function<bool(T, qint64)> &iterator, const std::function<void(int, EO)> &failureHandler, const std::function<void(QString, int, RestReply::Error)> &errorHandler, const std::function<void(QtJsonSerializer::Exception &)> &exceptionHandler, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if (index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index),
						   d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next<EO>()->onSucceeded([iterator, failureHandler, errorHandler, exceptionHandler, to, index](int, const Paging<T> &paging) {
					  if (paging.isValid())
						  paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, index);
				  })
			->onFailed(failureHandler)
			->onError(errorHandler)
			->onSerializeException(exceptionHandler);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(QObject *scope, const std::function<bool(T, qint64)> &iterator, const std::function<void(int, EO)> &failureHandler, const std::function<void(QString, int, RestReply::Error)> &errorHandler, const std::function<void(QtJsonSerializer::Exception &)> &exceptionHandler, qint64 to, qint64 from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if (index < 0)
		return;

	//continue to the next one
	auto max = calcMax(to);
	if(index < max && d->iPaging->hasNext()) {
		qCDebug(logPaging, "Requesting next paging object with offset %s as %s", QString::number(index),
						   d->iPaging->next().toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo));
		next<EO>()->onSucceeded(scope, [scope, iterator, failureHandler, errorHandler, exceptionHandler, to, index](int, const Paging<T> &paging) {
					  if (paging.isValid())
						  paging.iterate(scope, iterator, failureHandler, errorHandler, exceptionHandler, to, index);
				  })
			->onFailed(scope, failureHandler)
			->onError(scope, errorHandler)
			->onSerializeException(exceptionHandler);
	}
}

template<typename T>
QVariantMap Paging<T>::properties() const
{
	return d->iPaging->properties();
}

template<typename T>
void Paging<T>::deleteAllItems() const
{
	__private::MetaComponent<T>::deleteAllLater(d->data);
}

template<typename T>
qint64 Paging<T>::internalIterate(const std::function<bool (T, qint64)> &iterator, qint64 to, qint64 from) const
{
	// handle all items in this paging
	auto offset = d->iPaging->offset();
	auto count = d->data.size();
	int start = 0;
	int max = count;
	if (offset >= 0) {  // has indexes
		start = static_cast<int>(std::max(from, offset) - offset);
		if (to >= 0)
			max = static_cast<int>(std::min(to, offset + count) - offset);
	}

	qCDebug(logPaging, "iterating over available range [%s:%s]",
			QString::number(offset + start),
			QString::number(offset + max - 1));

	// delete unused items caused by from
	for (auto j = 0; j < start; ++j)
		__private::MetaComponent<T>::deleteLater(d->data.value(j));

	// iterate over used items
	int i;
	auto canceled = false;
	for (i = start; i < max; ++i) {
		auto item = d->data.value(i);
		auto index = offset >= 0 ? offset + i : -1ll;
		if (!iterator(item, index)) {
			qCDebug(logPaging, "Iterator stopped paging iteration at index %s", 
					QString::number(index));
			canceled = true;
			break;
		}
	}

	//delete all unused items caused by to
	for (auto j = i; j < count; ++j)
		__private::MetaComponent<T>::deleteLater(d->data.value(j));

	if (canceled)
		return -1;
	else if (offset >= 0)
		return offset + i;
	else
		return 0;
}

template<typename T>
qint64 Paging<T>::calcMax(qint64 to) const
{
	if (d->iPaging->offset() >= 0) {
		if (to >= 0)
			return std::min(to, d->iPaging->total());
		else
			return d->iPaging->total();
	} else
		return std::numeric_limits<qint64>::max();
}

}

#endif // QTRESTCLIENT_PAGING_H
