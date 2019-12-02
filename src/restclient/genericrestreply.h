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
template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReply : public RestReply
{
	static_assert(MetaComponent<DataClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::value, "ErrorClassType must inherit QObject or have Q_GADGET!");
public:
	//! Creates a generic reply based on a network reply and for a client
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copybrief RestReply::onSucceeded(const std::function<void(int, QJsonObject)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(const std::function<void(int, DataClassType)> &handler);
	//! @copybrief GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(QObject *scope, const std::function<void(int, DataClassType)> &handler);
	//! @copybrief RestReply::onFailed(const std::function<void(int, QJsonObject)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onFailed(const std::function<void(int, ErrorClassType)> &handler);
	//! @copybrief GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onFailed(QObject *scope, const std::function<void(int, ErrorClassType)> &handler);
	//! Set a handler to be called on deserialization exceptions
	GenericRestReply<DataClassType, ErrorClassType> *onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler);
	//! @copybrief RestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
																 const std::function<QString(ErrorClassType, int)> &failureTransformer = {});
	//! @copybrief GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<DataClassType, ErrorClassType> *onAllErrors(QObject *scope,
																 const std::function<void(QString, int, ErrorType)> &handler,
																 const std::function<QString(ErrorClassType, int)> &failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc RestReply::onCompleted(const std::function<void(int)> &)
	GenericRestReply<DataClassType, ErrorClassType> *onCompleted(const std::function<void(int)> &handler);
	//! @copydoc RestReply::onCompleted(QObject *, const std::function<void(int)> &)
	GenericRestReply<DataClassType, ErrorClassType> *onCompleted(QObject *scope, const std::function<void(int)> &handler);
	//! @copydoc RestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<DataClassType, ErrorClassType> *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc RestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<DataClassType, ErrorClassType> *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc RestReply::disableAutoDelete
	GenericRestReply<DataClassType, ErrorClassType> *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<DataClassType, ErrorClassType> awaitable();

private:
	RestClient *client;
	std::function<void(QtJsonSerializer::Exception &)> exceptionHandler;
};

//! @note This class is a simple specialization for replies withput a result. It behaves the same as a normal GenericRestReply, however,
//! there is no DataClassType, just void, for cases where you don't care about the result itself, only the code.
//! @copydoc QtRestClient::GenericRestReply
template <typename ErrorClassType>
class GenericRestReply<void, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<ErrorClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<void, ErrorClassType> *onSucceeded(const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, const std::function<void(int, DataClassType)>&)
	GenericRestReply<void, ErrorClassType> *onSucceeded(QObject *scope, const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<void, ErrorClassType> *onFailed(const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onFailed(QObject*, const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<void, ErrorClassType> *onFailed(QObject *scope, const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<void, ErrorClassType> *onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler);
	//! @copydoc GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<void, ErrorClassType> *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
														const std::function<QString(ErrorClassType, int)> &failureTransformer = {});
	//! @copydoc GenericRestReply::onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<void, ErrorClassType> *onAllErrors(QObject *scope,
														const std::function<void(QString, int, ErrorType)> &handler,
														const std::function<QString(ErrorClassType, int)> &failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc GenericRestReply::onCompleted(const std::function<void(int)> &)
	GenericRestReply<void, ErrorClassType> *onCompleted(const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onCompleted(QObject *, const std::function<void(int)> &)
	GenericRestReply<void, ErrorClassType> *onCompleted(QObject *scope, const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<void, ErrorClassType> *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<void, ErrorClassType> *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<void, ErrorClassType> *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<void, ErrorClassType> awaitable();

private:
	RestClient *client;
	std::function<void(QtJsonSerializer::Exception &)> exceptionHandler;
};

//! @note This class is a simple specialization for list types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create requests and replies with lists (using QList<>). Of cause, the DataClassType, as generic parameter
//! for the QList, must fullfill the same restrictions as for the normal one
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<DataClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSucceeded(const std::function<void(int, QList<DataClassType>)> &handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, const std::function<void(int, DataClassType)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSucceeded(QObject *scope, const std::function<void(int, QList<DataClassType>)> &handler);
	//! @copydoc GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onFailed(const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onFailed(QObject*, const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onFailed(QObject *scope, const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler);
	//! @copydoc GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
																		const std::function<QString(ErrorClassType, int)> &failureTransformer = {});
	//! @copydoc GenericRestReply::onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onAllErrors(QObject *scope,
																		const std::function<void(QString, int, ErrorType)> &handler,
																		const std::function<QString(ErrorClassType, int)> &failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc GenericRestReply::onCompleted(const std::function<void(int)> &)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onCompleted(const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onCompleted(QObject *, const std::function<void(int)> &)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onCompleted(QObject *scope, const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<QList<DataClassType>, ErrorClassType> *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<QList<DataClassType>, ErrorClassType> awaitable();

private:
	RestClient *client;
	std::function<void(QtJsonSerializer::Exception &)> exceptionHandler;
};

//! @note This class is a simple specialization for paging types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create replies with paging logic (using Paging<>). Of cause, the DataClassType, as generic parameter
//! for the Paging, must fullfill the same restrictions as for the normal one. Paging allows you to only get a part of a
//! list form the server, and allows you to iterate over those results.
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<DataClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded(const std::function<void(int, DataClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(const std::function<void(int, Paging<DataClassType>)> &handler);
	//! @copydoc GenericRestReply::onSucceeded(QObject*, const std::function<void(int, DataClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(QObject *scope, const std::function<void(int, Paging<DataClassType>)> &handler);
	//! @copydoc GenericRestReply::onFailed(const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onFailed(QObject*, const std::function<void(int, ErrorClassType)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(QObject *scope, const std::function<void(int, ErrorClassType)> &handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler);
	//! @copydoc GenericRestReply::onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
																		 const std::function<QString(ErrorClassType, int)> &failureTransformer = {});
	//! @copydoc GenericRestReply::onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&, const std::function<QString(ErrorClassType, int)>&)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onAllErrors(QObject *scope,
																		 const std::function<void(QString, int, ErrorType)> &handler,
																		 const std::function<QString(ErrorClassType, int)> &failureTransformer = {});

	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(const std::function<bool(DataClassType, int)> &iterator, int to = -1, int from = 0);
	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(QObject *scope, const std::function<bool(DataClassType, int)> &iterator, int to = -1, int from = 0);

	//overshadowing, for the right return type only..
	//! @copydoc GenericRestReply::onCompleted(const std::function<void(int)> &)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onCompleted(const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onCompleted(QObject *, const std::function<void(int)> &)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onCompleted(QObject *scope, const std::function<void(int)> &handler);
	//! @copydoc GenericRestReply::onError(const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::onError(QObject *, const std::function<void(QString, int, ErrorType)> &)
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *disableAutoDelete();

	//! @copybrief RestReply::awaitable
	GenericRestReplyAwaitable<Paging<DataClassType>, ErrorClassType> awaitable();

private:
	RestClient *client;
	std::function<void(int, ErrorClassType)> failureHandler;
	std::function<void(QString, int, ErrorType)> errorHandler;
	std::function<void(QtJsonSerializer::Exception &)> exceptionHandler;
};

} //end namespace, because of include!

//include after delecation, to allow foreward declared types
#include "QtRestClient/paging.h"

namespace QtRestClient {

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	client(client),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(const std::function<void (int, DataClassType)> &handler)
{
	return onSucceeded(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(QObject *scope, const std::function<void (int, DataClassType)> &handler)
{
	if (!handler)
		return this;
	RestReply::onSucceeded(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [handler, code](std::nullopt_t) {
							   handler(code, DataClassType{});
						   },
						   [this, handler, code](const auto &data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<DataClassType>()).template value<DataClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onFailed(const std::function<void (int, ErrorClassType)> &handler)
{
	return onFailed(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onFailed(QObject *scope, const std::function<void (int, ErrorClassType)> &handler)
{
	if(!handler)
		return this;
	RestReply::onFailed(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [=](std::nullopt_t) {
							   handler(code, {});
						   },
						   [=](auto data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler)
{
	exceptionHandler = std::move(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onAllErrors(const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onAllErrors(QObject *scope, const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	this->onFailed(scope, [=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	this->onSerializeException([handler](QtJsonSerializer::Exception exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onCompleted(const std::function<void (int)> &handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onCompleted(QObject *scope, const std::function<void (int)> &handler)
{
	RestReply::onCompleted(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onError(const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return this;
}

// ------------- Implementation void -------------

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	client(client),
	exceptionHandler()
{
	setAllowEmptyReplies(true);
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSucceeded(const std::function<void (int)> &handler)
{
	return onSucceeded(this, handler);
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSucceeded(QObject *scope, const std::function<void (int)> &handler)
{
	RestReply::onSucceeded(scope, handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onFailed(const std::function<void (int, ErrorClassType)> &handler)
{
	return onFailed(this, handler);
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onFailed(QObject *scope, const std::function<void (int, ErrorClassType)> &handler)
{
	if (!handler)
		return this;
	RestReply::onFailed(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [=](std::nullopt_t) {
							   handler(code, {});
						   },
						   [=](auto data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler)
{
	exceptionHandler = std::move(handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onAllErrors(const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onAllErrors(QObject *scope, const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	this->onFailed(scope, [=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	this->onSerializeException([handler](QtJsonSerializer::Exception exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onCompleted(const std::function<void (int)> &handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onCompleted(QObject *scope, const std::function<void (int)> &handler)
{
	RestReply::onCompleted(scope, handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onError(const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(scope, handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return this;
}

// ------------- Implementation List of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	client(client),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(const std::function<void (int, QList<DataClassType>)> &handler)
{
	return onSucceeded(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(QObject *scope, const std::function<void (int, QList<DataClassType>)> &handler)
{
	if(!handler)
		return this;
	RestReply::onSucceeded(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [handler, code](std::nullopt_t) {
							   handler(code, QList<DataClassType>{});
						   },
						   [this, handler, code](const auto &data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<QList<DataClassType>>()).template value<QList<DataClassType>>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(const std::function<void (int, ErrorClassType)> &handler)
{
	return onFailed(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(QObject *scope, const std::function<void (int, ErrorClassType)> &handler)
{
	if (!handler)
		return this;
	RestReply::onFailed(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [=](std::nullopt_t) {
							   handler(code, {});
						   },
						   [=](auto data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler)
{
	exceptionHandler = std::move(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onAllErrors(const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onAllErrors(QObject *scope, const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	this->onFailed(scope, [=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	this->onSerializeException([handler](QtJsonSerializer::Exception exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onCompleted(const std::function<void (int)> &handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onCompleted(QObject *scope, const std::function<void (int)> &handler)
{
	RestReply::onCompleted(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onError(const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	RestReply::onError(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return this;
}

// ------------- Implementation Paging of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	client(client),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(const std::function<void (int, Paging<DataClassType>)> &handler)
{
	return onSucceeded(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(QObject *scope, const std::function<void (int, Paging<DataClassType>)> &handler)
{
	if(!handler)
		return this;
	RestReply::onSucceeded(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [handler, code](std::nullopt_t) {
							   handler(code, Paging<DataClassType>{});
						   },
						   [this, handler, code](const auto &data) {
							   auto iPaging = client->pagingFactory()->createPaging(client->serializer(), data);
							   auto pData = client->serializer()->deserializeGeneric(std::visit(__private::overload {
																									[](const QCborArray &data) -> QCborValue {
																										return data;
																									},
																									[](const QJsonArray &data) -> QJsonValue {
																										return data;
																									}
																								}, iPaging->items()), qMetaTypeId<QList<DataClassType>>()).template value<QList<DataClassType>>();
							   handler(code, Paging<DataClassType>(iPaging, std::move(pData), client));
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onFailed(const std::function<void (int, ErrorClassType)> &handler)
{
	return onFailed(this, handler);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onFailed(QObject *scope, const std::function<void (int, ErrorClassType)> &handler)
{
	failureHandler = handler;
	if (!handler)
		return this;
	RestReply::onFailed(scope, [=](int code, const DataType &value){
		try {
			std::visit(__private::overload {
						   [=](std::nullopt_t) {
							   handler(code, {});
						   },
						   [=](auto data) {
							   handler(code, client->serializer()->deserializeGeneric(data, qMetaTypeId<ErrorClassType>()).template value<ErrorClassType>());
						   }
					   }, value);
		} catch (QtJsonSerializer::Exception &e) {
			if (exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSerializeException(std::function<void(QtJsonSerializer::Exception&)>handler)
{
	exceptionHandler = std::move(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onAllErrors(const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onAllErrors(QObject *scope, const std::function<void (QString, int, ErrorType)> &handler, const std::function<QString (ErrorClassType, int)> &failureTransformer)
{
	this->onFailed(scope, [=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(scope, handler);
	this->onSerializeException([handler](QtJsonSerializer::Exception exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(const std::function<bool (DataClassType, int)> &iterator, int to, int from)
{
	return iterate(this, iterator, to, from);
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(QObject *scope, const std::function<bool (DataClassType, int)> &iterator, int to, int from)
{
	return onSucceeded(scope, [=](int, Paging<DataClassType> paging){
		paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, from);
	});
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onCompleted(const std::function<void (int)> &handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onCompleted(QObject *scope, const std::function<void (int)> &handler)
{
	RestReply::onCompleted(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onError(const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	errorHandler = handler;
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	errorHandler = handler;
	RestReply::onError(scope, handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::disableAutoDelete()
{
	RestReply::disableAutoDelete();
	return this;
}

}

#endif // QTRESTCLIENT_GENERICRESTREPLY_H
