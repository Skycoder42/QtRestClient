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

//! A class to handle generic replies for generic requests
template <typename DataClassType, typename ErrorClassType>
class GenericRestReplyBase : public RestReply
{
public:
	using TInstance = GenericRestReply<DataClassType, ErrorClassType>;

	//! @copybrief RestReply::onFailed(const std::function<void(int, QJsonObject)>&)
	TInstance *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copybrief GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	virtual TInstance *onFailed(QObject *scope, std::function<void(int, ErrorClassType)> handler);
	//! Set a handler to be called on deserialization exceptions
	TInstance *onSerializeException(std::function<void(QtJsonSerializer::Exception&)> handler);
	//! @copybrief RestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	TInstance *onAllErrors(const std::function<void(QString, int, Error)> &handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});
	//! @copybrief GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	TInstance *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, Error)> &handler,
						   std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc RestReply::onCompleted(const std::function<void(int)> &)
	TInstance *onCompleted(std::function<void(int)> handler);
	//! @copydoc RestReply::onCompleted(QObject *, const std::function<void(int)> &)
	TInstance *onCompleted(QObject *scope, std::function<void(int)> handler);
	//! @copydoc RestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	TInstance *onError(std::function<void(QString, int, Error)> handler);
	//! @copydoc RestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	virtual TInstance *onError(QObject *scope, std::function<void(QString, int, Error)> handler);
	//! @copydoc RestReply::disableAutoDelete
	TInstance *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<DataClassType, ErrorClassType> awaitable();

protected:
	GenericRestReplyBase(QNetworkReply *networkReply,
						 RestClient *client,
						 QObject *parent);

	RestClient *_client;
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

	//! @copybrief RestReply::onSucceeded(const std::function<void(int, QJsonObject)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(std::function<void(int, DataClassType)> handler);
	//! @copybrief GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(QObject *scope, std::function<void(int, DataClassType)> handler);
};

//! @note This class is a simple specialization for replies withput a result. It behaves the same as a normal GenericRestReply, however,
//! there is no DataClassType, just void, for cases where you don't care about the result itself, only the code.
//! @copydoc QtRestClient::GenericRestReply
template <typename ErrorClassType>
class GenericRestReply<void, ErrorClassType> : public GenericRestReplyBase<void, ErrorClassType>
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
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>
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
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(std::function<bool(DataClassType, qint64)> iterator,
																	 qint64 to = -1,
																	 qint64 from = 0);
	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(QObject *scope,
																	 std::function<bool(DataClassType, qint64)> iterator,
																	 qint64 to = -1,
																	 qint64 from = 0);

	// overshadow for setters
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(QObject *scope, std::function<void(int, ErrorClassType)> handler) final;
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
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onError(std::function<void (QString, int, RestReply::Error)> handler)
{
	return onError(this, std::move(handler));
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::onError(QObject *scope, std::function<void (QString, int, RestReply::Error)> handler)
{
	RestReply::onError(scope, std::move(handler));
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
typename GenericRestReplyBase<DataClassType, ErrorClassType>::TInstance *GenericRestReplyBase<DataClassType, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return static_cast<TInstance*>(this);
}

template <typename DataClassType, typename ErrorClassType>
GenericRestReplyBase<DataClassType, ErrorClassType>::GenericRestReplyBase(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  RestReply{networkReply, parent},
	  _client{client}
{}

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	  GenericRestReplyBase<DataClassType, ErrorClassType>{networkReply, client, parent}
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
	  GenericRestReplyBase<void, ErrorClassType>{networkReply, client, parent}
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
	  GenericRestReplyBase<Paging<DataClassType>, ErrorClassType>{networkReply, client, parent}
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
