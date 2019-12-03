#ifndef QTRESTCLIENT_GENERICRESTREPLY_H
#define QTRESTCLIENT_GENERICRESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/metacomponent.h"

#include <QtJsonSerializer/jsonserializer.h>
#include <type_traits>

namespace QtRestClient {

template <typename DataClassType, typename ErrorClassType>
class GenericRestReplyAwaitable;

//! A class to handle generic replies for generic requests
template <typename TInstance, typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReplyBase : public RestReply
{
public:
	//! @copybrief RestReply::onFailed(const std::function<void(int, QJsonObject)>&)
	TInstance *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copybrief GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	TInstance *onFailed(QObject *scope, std::function<void(int, ErrorClassType)> handler);
	//! Set a handler to be called on deserialization exceptions
	TInstance *onSerializeException(std::function<void(QtJsonSerializer::Exception&)> handler);
	//! @copybrief RestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	TInstance *onAllErrors(std::function<void(QString, int, Error)> handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});
	//! @copybrief GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	TInstance *onAllErrors(QObject *scope,
						   std::function<void(QString, int, Error)> handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc RestReply::onCompleted(const std::function<void(int)> &)
	TInstance *onCompleted(std::function<void(int)> handler);
	//! @copydoc RestReply::onCompleted(QObject *, const std::function<void(int)> &)
	TInstance *onCompleted(QObject *scope, std::function<void(int)> handler);
	//! @copydoc RestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	TInstance *onError(std::function<void(QString, int, Error)> handler);
	//! @copydoc RestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	TInstance *onError(QObject *scope, std::function<void(QString, int, Error)> handler);
	//! @copydoc RestReply::disableAutoDelete
	TInstance *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<DataClassType, ErrorClassType> awaitable();

protected:
	GenericRestReplyBase(QNetworkReply *networkReply,
						 RestClient *client,
						 QObject *parent);

	RestClient *_client;
	std::function<void(int, ErrorClassType)> _failureHandler;
	std::function<void(QString, int, Error)> _errorHandler;
	std::function<void(QtJsonSerializer::Exception &)> _exceptionHandler;
};

//! A class to handle generic replies for generic requests
template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReply : public GenericRestReplyBase<GenericRestReply<DataClassType, ErrorClassType>, DataClassType, ErrorClassType>
{
public:
	//! Creates a generic reply based on a network reply and for a client
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copybrief RestReply::onSucceeded(const std::function<void(int, QJsonObject)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(std::function<void(int, DataClassType)> handler);
	//! @copybrief GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int, DataClassType)> handler);
};

//! @note This class is a simple specialization for replies withput a result. It behaves the same as a normal GenericRestReply, however,
//! there is no DataClassType, just void, for cases where you don't care about the result itself, only the code.
//! @copydoc QtRestClient::GenericRestReply
template <typename ErrorClassType>
class GenericRestReply<void, ErrorClassType> : public GenericRestReplyBase<GenericRestReply<void, ErrorClassType>, void, ErrorClassType>
{
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<void, ErrorClassType> *onSucceeded(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, const std::function<void(int, DataClassType)>&)
	GenericRestReply<void, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int)> handler);
};

//! @note This class is a simple specialization for paging types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create replies with paging logic (using Paging<>). Of cause, the DataClassType, as generic parameter
//! for the Paging, must fullfill the same restrictions as for the normal one. Paging allows you to only get a part of a
//! list form the server, and allows you to iterate over those results.
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public GenericRestReplyBase<GenericRestReply<Paging<DataClassType>, ErrorClassType>, Paging<DataClassType>, ErrorClassType>
{
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(int, Paging<DataClassType>)> handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, const std::function<void(int, DataClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int, Paging<DataClassType>)> handler);

	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(std::function<bool(DataClassType, int)> iterator, int to = -1, int from = 0);
	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(QObject *scope, std::function<bool(DataClassType, int)> iterator, int to = -1, int from = 0);

};

} //end namespace, because of include!

//include after delecation, to allow foreward declared types
#include "QtRestClient/paging.h"

namespace QtRestClient {

// ------------- Implementation base class -------------

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	return onFailed(this, std::move(handler));
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onFailed(QObject *scope, std::function<void (int, ErrorClassType)> handler)
{
	RestReply::onFailed(scope, [this, handler](int code, const DataType &value){
		std::visit(__private::overload {
					   [&](std::nullopt_t) {
						   handler(code, ErrorClassType{});
					   },
					   [&](auto data) {
						   try {
							   handler(code, _client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   } catch (QtJsonSerializer::DeserializationException &e) {
							   if (_exceptionHandler)
								   _exceptionHandler(e);
							   handler(code, ErrorClassType{});
						   }
					   }
				   }, value);
	});
	_failureHandler = std::move(handler);
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)> handler)
{
	_exceptionHandler = std::move(handler);
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onAllErrors(std::function<void (QString, int, Error)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	return onAllErrors(this, std::move(handler), std::move(failureTransformer));
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onAllErrors(QObject *scope, std::function<void (QString, int, Error)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed(scope, [handler, failureTransformer](int code, ErrorClassType obj){
		if (failureTransformer)
			handler(failureTransformer(obj, code), code, Failure);
		else
			handler(QString(), code, Failure);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	this->onSerializeException([handler](QtJsonSerializer::Exception exception){
		handler(QString::fromUtf8(exception.what()), 0, Deserialization);
	});
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	return onCompleted(this, handler);
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onCompleted(QObject *scope, std::function<void (int)> handler)
{
	RestReply::onCompleted(scope, handler);
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onError(std::function<void (QString, int, RestReply::Error)> handler)
{
	return onError(this, handler);
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::onError(QObject *scope, std::function<void (QString, int, RestReply::Error)> handler)
{
	RestReply::onError(scope, handler);
	_errorHandler = std::move(handler);
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
TInstance *GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return this;
}

template <typename TInstance, typename DataClassType, typename ErrorClassType>
GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::GenericRestReplyBase(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  RestReply{networkReply, parent},
	  _client{client}
{}

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  GenericRestReplyBase<GenericRestReply<DataClassType, ErrorClassType>, DataClassType, ErrorClassType>{networkReply, client, parent}
{}

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
	  GenericRestReplyBase<GenericRestReply<void, ErrorClassType>, void, ErrorClassType>{networkReply, client, parent}
{
	this->setAllowEmptyReplies(true);
}

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
	  GenericRestReplyBase<GenericRestReply<Paging<DataClassType>, ErrorClassType>, Paging<DataClassType>, ErrorClassType>{networkReply, client, parent}
{}

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
																										   [](const QCborArray &data) -> QCborValue {
																											   return data;
																										   },
																										   [](const QJsonArray &data) -> QJsonValue {
																											   return data;
																										   }
																									   }, iPaging->items()), qMetaTypeId<QList<DataClassType>>()).template value<QList<DataClassType>>();
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(std::function<bool (DataClassType, int)> iterator, int to, int from)
{
	return onSucceeded(this, [this, it = std::move(iterator), to, from](int, Paging<DataClassType> paging){
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(QObject *scope, std::function<bool (DataClassType, int)> iterator, int to, int from)
{
	return onSucceeded(scope, [this, scope, it = std::move(iterator), to, from](int, Paging<DataClassType> paging){
		paging.iterate(scope, it, this->_failureHandler, this->_errorHandler, this->_exceptionHandler, to, from);
	});
}

}

#endif // QTRESTCLIENT_GENERICRESTREPLY_H
