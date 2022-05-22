#ifndef QTRESTCLIENT_GENERICRESTREPLY_H
#define QTRESTCLIENT_GENERICRESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/metacomponent.h"

#include <type_traits>

#include <QtJsonSerializer/serializerbase.h>

namespace QtRestClient {

template <typename DataClassType, typename ErrorClassType>
class GenericRestReplyAwaitable;

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply;

//! The base class for GenericRestReply specializations
template <typename DataClassType, typename ErrorClassType>
class GenericRestReplyBase : public RestReply
{
public:
	//! The specialized GenericRestReply that is associated with the reply base
	using TInstance = GenericRestReply<DataClassType, ErrorClassType>;

	//! @copybrief QtRestClient::RestReply::onFailed(TFn&&)
	TInstance *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copybrief QtRestClient::GenericRestReplyBase::onFailed(std::function<void(int, ErrorClassType)>)
	virtual TInstance *onFailed(QObject *scope, std::function<void(int, ErrorClassType)> handler);
	//! Set a handler to be called on deserialization exceptions
	TInstance *onSerializeException(std::function<void(QtJsonSerializer::Exception&)> handler);
	//! @copybrief QtRestClient::RestReply::onAllErrors(const std::function<void(QString, int, Error)>&, TFn&&)
	TInstance *onAllErrors(const std::function<void(QString, int, Error)> &handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});
	//! @copybrief QtRestClient::GenericRestReplyBase::onAllErrors(const std::function<void(QString, int, Error)>&, std::function<QString(ErrorClassType, int)>)
	TInstance *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, Error)> &handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc QtRestClient::RestReply::onCompleted(TFn&&)
	TInstance *onCompleted(std::function<void(int)> handler);
	//! @copydoc QtRestClient::RestReply::onCompleted(QObject *, TFn&&)
	TInstance *onCompleted(QObject *scope, std::function<void(int)> handler);
	//! @copydoc QtRestClient::RestReply::onError(std::function<void(QString, int, Error)>)
	TInstance *onError(std::function<void(QString, int, Error)> handler);
	//! @copydoc QtRestClient::RestReply::onError(QObject *, std::function<void(QString, int, Error)>)
	virtual TInstance *onError(QObject *scope, std::function<void(QString, int, RestReply::Error)> handler);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @copydoc QtRestClient::RestReply::makeAsync
	TInstance *makeAsync(QThreadPool *threadPool = QThreadPool::globalInstance());
#endif
	//! @copydoc QtRestClient::RestReply::disableAutoDelete
	TInstance *disableAutoDelete();

	//! @copybrief QtRestClient::RestReply::awaitable
	GenericRestReplyAwaitable<DataClassType, ErrorClassType> awaitable();

protected:
	//! @private
	GenericRestReplyBase(QNetworkReply *networkReply,
						 RestClient *client,
						 QObject *parent);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @private
	GenericRestReplyBase(const QFuture<QNetworkReply*> &networkReplyFuture,
						 RestClient *client,
						 QObject *parent);
#endif

	//! @private
	RestClient *_client;
	//! @private
	std::function<void(QtJsonSerializer::Exception &)> _exceptionHandler;
};

//! A class to handle generic replies for generic requests
template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReply : public GenericRestReplyBase<DataClassType, ErrorClassType>
{
public:
	//! Creates a generic reply based on a network reply and for a client
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! Creates a generic reply based on a network reply future and for a client
	GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture,
					 RestClient *client,
					 QObject *parent = nullptr);
#endif

	//! @copybrief RestReply::onSucceeded(TFn&&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(std::function<void(int, DataClassType)> handler);
	//! @copybrief GenericRestReply::onSucceeded(std::function<void(int, DataClassType)>)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int, DataClassType)> handler);
};

//! @note This class is a simple specialization for replies withput a result. It behaves the same as a normal GenericRestReply, however,
//! there is no DataClassType, just void, for cases where you don't care about the result itself, only the code.
//! @copydoc QtRestClient::GenericRestReply
template <typename ErrorClassType>
class GenericRestReply<void, ErrorClassType> : public GenericRestReplyBase<void, ErrorClassType>
{
public:
	//! @copydoc GenericRestReply::GenericRestReply(QNetworkReply*, RestClient*, QObject*)
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @copydoc GenericRestReply::GenericRestReply(const QFuture<QNetworkReply*> &, RestClient*, QObject*)
	GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture,
					 RestClient *client,
					 QObject *parent = nullptr);
#endif

	//! @copydoc GenericRestReply::onSucceeded(std::function<void(int, DataClassType)>)
	GenericRestReply<void, ErrorClassType> *onSucceeded(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, std::function<void(int, DataClassType)>)
	GenericRestReply<void, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int)> handler);
};

//! @note This class is a simple specialization for paging types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create replies with paging logic (using Paging<>). Of cause, the DataClassType, as generic parameter
//! for the Paging, must fullfill the same restrictions as for the normal one. Paging allows you to only get a part of a
//! list form the server, and allows you to iterate over those results.
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>
{
public:
	//! @copydoc GenericRestReply::GenericRestReply(QNetworkReply*, RestClient*, QObject*)
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @copydoc GenericRestReply::GenericRestReply(const QFuture<QNetworkReply*> &, RestClient*, QObject*)
	GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture,
					 RestClient *client,
					 QObject *parent = nullptr);
#endif

	//! @copydoc GenericRestReply::onSucceeded(std::function<void(int, DataClassType)>)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(int, Paging<DataClassType>)> handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, std::function<void(int, DataClassType)>)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int, Paging<DataClassType>)> handler);

	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(std::function<bool(DataClassType, qint64)> iterator,
																	 qint64 to = -1,
																	 qint64 from = 0);
	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(QObject *scope,
																	 std::function<bool(DataClassType, qint64)> iterator,
																	 qint64 to = -1,
																	 qint64 from = 0);

	using GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>::onFailed;
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(QObject *scope, std::function<void(int, ErrorClassType)> handler) final;
	using GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>::onError;
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onError(QObject *scope, std::function<void(QString, int, RestReply::Error)> handler) final;

private:
	std::function<void(int, ErrorClassType)> _failureHandler;
	std::function<void(QString, int, RestReply::Error)> _errorHandler;
};

} //end namespace, because of include!

//include after delecation, to allow foreward declared types
#include "QtRestClient/paging.h"

namespace QtRestClient {

// ------------- Implementation base class -------------

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	return onFailed(this, std::move(handler));
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onFailed(QObject *scope, std::function<void (int, ErrorClassType)> handler)
{
	RestReply::onFailed(scope, [this, xHandler = std::move(handler)](int code, const DataType &value){
		std::visit(__private::overload {
					   [&](std::nullopt_t) {
						   xHandler(code, ErrorClassType{});
					   },
					   [&](auto data) {
						   try {
							   xHandler(code, _client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   } catch (QtJsonSerializer::DeserializationException &e) {
							   if (_exceptionHandler)
								   _exceptionHandler(e);
							   xHandler(code, ErrorClassType{});
						   }
					   }
				   }, value);
	});
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)> handler)
{
	_exceptionHandler = std::move(handler);
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onAllErrors(const std::function<void (QString, int, Error)> &handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	return onAllErrors(this, handler, std::move(failureTransformer));
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onAllErrors(QObject *scope, const std::function<void (QString, int, Error)> &handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onSerializeException([handler](QtJsonSerializer::Exception &exception){
		handler(QString::fromUtf8(exception.what()), 0, Error::Deserialization);
	});
	this->onFailed(scope, [handler, xFt = std::move(failureTransformer)](int code, ErrorClassType obj){
		if (xFt)
			handler(xFt(obj, code), code, Error::Failure);
		else
			handler({}, code, Error::Failure);
		__private::MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	return onCompleted(this, std::move(handler));
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onCompleted(QObject *scope, std::function<void (int)> handler)
{
	RestReply::onCompleted(scope, std::move(handler));
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onError(std::function<void (QString, int, Error)> handler)
{
	return onError(this, std::move(handler));
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onError(QObject *scope, std::function<void (QString, int, RestReply::Error)> handler)
{
	RestReply::onError(scope, std::move(handler));
	return static_cast<TInstance*>(this);
}

#ifdef QT_RESTCLIENT_USE_ASYNC
template<typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::makeAsync(QThreadPool *threadPool)
{
	RestReply::makeAsync(threadPool);
	return static_cast<TInstance*>(this);
}
#endif

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return static_cast<TInstance*>(this);
}

#ifdef QT_RESTCLIENT_USE_ASYNC
template <typename DataClassType, typename ErrorClassType>
GenericRestReplyBase<DataClassType, ErrorClassType>::GenericRestReplyBase(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply{networkReply, client->asyncPool(), parent},
	_client{client}
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyBase<DataClassType, ErrorClassType>::GenericRestReplyBase(const QFuture<QNetworkReply*> &networkReplyFuture, RestClient *client, QObject *parent) :
	RestReply{networkReplyFuture, client->asyncPool(), parent},
	_client{client}
{}
#else
template <typename DataClassType, typename ErrorClassType>
GenericRestReplyBase<DataClassType, ErrorClassType>::GenericRestReplyBase(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply{networkReply, parent},
	_client{client}
{}
#endif

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	GenericRestReplyBase<DataClassType, ErrorClassType>{networkReply, client, parent}
{}

#ifdef QT_RESTCLIENT_USE_ASYNC
template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture, RestClient *client, QObject *parent) :
	GenericRestReplyBase<DataClassType, ErrorClassType>{networkReplyFuture, client, parent}
{}
#endif

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<void (int, DataClassType)> handler)
{
	return onSucceeded(this, std::move(handler));
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(QObject *scope, std::function<void (int, DataClassType)> handler)
{
	RestReply::onSucceeded(scope, [this, xFn = std::move(handler)](int code, const RestReply::DataType &value){
		try {
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   xFn(code, DataClassType{});
						   },
						   [&](const auto &data) {
							   xFn(code, this->_client->serializer()->deserializeGeneric(data, qMetaTypeId<DataClassType>()).template value<DataClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::DeserializationException &e) {
			if (this->_exceptionHandler)
				this->_exceptionHandler(e);
		}
	});
	return this;
}

// ------------- Implementation void -------------

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  GenericRestReplyBase<void, ErrorClassType>{networkReply, client, parent}
{
	this->setAllowEmptyReplies(true);
}

#ifdef QT_RESTCLIENT_USE_ASYNC
template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType>::GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture, RestClient *client, QObject *parent) :
	GenericRestReplyBase<void, ErrorClassType>{networkReplyFuture, client, parent}
{
	this->setAllowEmptyReplies(true);
}
#endif

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSucceeded(std::function<void (int)> handler)
{
	return onSucceeded(this, std::move(handler));
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSucceeded(QObject *scope, std::function<void (int)> handler)
{
	RestReply::onSucceeded(scope, std::move(handler));
	return this;
}

// ------------- Implementation Paging of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>{networkReply, client, parent}
{}

#ifdef QT_RESTCLIENT_USE_ASYNC
template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType>::GenericRestReply(const QFuture<QNetworkReply*> &networkReplyFuture, RestClient *client, QObject *parent) :
	GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>{networkReplyFuture, client, parent}
{}
#endif

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (int, Paging<DataClassType>)> handler)
{
	return onSucceeded(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(QObject *scope, std::function<void (int, Paging<DataClassType>)> handler)
{
	RestReply::onSucceeded(scope, [this, xFn = std::move(handler)](int code, const RestReply::DataType &value){
		try {
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   xFn(code, Paging<DataClassType>{});
						   },
						   [&](const auto &data) {
							   auto iPaging = this->_client->pagingFactory()->createPaging(this->_client->serializer(), data);
							   auto pData = this->_client->serializer()->deserializeGeneric(std::visit(__private::overload {
																										   [](const QCborArray &innerData) -> std::variant<QCborValue, QJsonValue> {
																											   return QCborValue{innerData};
																										   },
																										   [](const QJsonArray &innerData) -> std::variant<QCborValue, QJsonValue> {
																											   return QJsonValue{innerData};
																										   }
																									   }, iPaging->items()),
																							qMetaTypeId<QList<DataClassType>>())
												.template value<QList<DataClassType>>();
							   xFn(code, Paging<DataClassType>(iPaging, std::move(pData), this->_client));
						   }
					   }, value);
		} catch (QtJsonSerializer::DeserializationException &e) {
			if (this->_exceptionHandler)
				this->_exceptionHandler(e);
		}
	});
	return this;
}

/*!
@param iterator The iterator to be used by the Paging objects to iterate over the results
@param to The upper limit of how far the iteration should go (-1 means no limit)
@param from The lower limit from where the iteration should start

This method is a shortcut that waits for the reply to succeed and then performs the iteration on the paging object.
One advantage of this method is, that it automatically passes the error handlers of this reply onto the iteration.
Check the Paging::iterate method for more details

The iterators parameters are:
- One element of the deserialized Content of the paging replies (DataClassType)
- The index of the current element (int)
- _returns:_ `true` if the iteration should continue, `false` to cancel it prematurely

@sa Paging::iterate
*/
template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(std::function<bool (DataClassType, qint64)> iterator, qint64 to, qint64 from)
{
	return onSucceeded(this, [this, it = std::move(iterator), to, from](int, const Paging<DataClassType> &paging) {
		if (paging.isValid())
			paging.iterate(it, this->_failureHandler, this->_errorHandler, this->_exceptionHandler, to, from);
	});
}

/*!
@param scope (optional) A scope to limit the callback to
@param iterator The iterator to be used by the Paging objects to iterate over the results
@param to The upper limit of how far the iteration should go (-1 means no limit)
@param from The lower limit from where the iteration should start

This method is a shortcut that waits for the reply to succeed and then performs the iteration on the paging object.
One advantage of this method is, that it automatically passes the error handlers of this reply onto the iteration.
Check the Paging::iterate method for more details

The iterators parameters are:
- One element of the deserialized Content of the paging replies (DataClassType)
- The index of the current element (int)
- _returns:_ `true` if the iteration should continue, `false` to cancel it prematurely

@sa Paging::iterate
*/
template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(QObject *scope, std::function<bool (DataClassType, qint64)> iterator, qint64 to, qint64 from)
{
	return onSucceeded(scope, [this, scope, it = std::move(iterator), to, from](int, const Paging<DataClassType> &paging){
		if (paging.isValid())
			paging.iterate(scope, it, this->_failureHandler, this->_errorHandler, this->_exceptionHandler, to, from);
	});
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onFailed(QObject *scope, std::function<void (int, ErrorClassType)> handler)
{
	_failureHandler = std::move(handler);
	return GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>::onFailed(scope, _failureHandler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onError(QObject *scope, std::function<void (QString, int, RestReply::Error)> handler)
{
	_errorHandler = std::move(handler);
	return GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>::onError(scope, _errorHandler);
}

}

#endif // QTRESTCLIENT_GENERICRESTREPLY_H
