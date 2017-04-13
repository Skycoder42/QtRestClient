#ifndef QTRESTCLIENT_GENERICRESTREPLY_H
#define QTRESTCLIENT_GENERICRESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/metacomponent.h"

#include <QtJsonSerializer/qjsonserializer.h>
#include <type_traits>

namespace QtRestClient {

//! A class to handle generic replies for generic requests
template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReply : public RestReply
{
	static_assert(MetaComponent<DataClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::is_meta::value, "ErrorClassType must inherit QObject or have Q_GADGET!");
public:
	//! Creates a generic reply based on a network reply and for a client
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copybrief RestReply::onSucceeded(std::function<void(int, QJsonObject)>)
	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(std::function<void(int, DataClassType)> handler);
	//! @copybrief RestReply::onFailed(std::function<void(int, QJsonObject)>)
	GenericRestReply<DataClassType, ErrorClassType> *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! Set a handler to be called on deserialization exceptions
	GenericRestReply<DataClassType, ErrorClassType> *onSerializeException(std::function<void(QJsonSerializerException &)> handler);
	//! @copybrief onAllErrors(std::function<void(QString, int, ErrorType)>, std::function<QString(QJsonObject, int)>)
	GenericRestReply<DataClassType, ErrorClassType> *onAllErrors(std::function<void(QString, int, ErrorType)> handler,
																 std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc RestReply::onCompleted
	GenericRestReply<DataClassType, ErrorClassType> *onCompleted(std::function<void(int)> handler);
	//! @copydoc RestReply::onError
	GenericRestReply<DataClassType, ErrorClassType> *onError(std::function<void(QString, int, ErrorType)> handler);
	//! @copydoc RestReply::disableAutoDelete
	GenericRestReply<DataClassType, ErrorClassType> *disableAutoDelete();

private:
	RestClient *client;
	std::function<void(QJsonSerializerException &)> exceptionHandler;
};

//! @note This class is a simple specialization for replies withput a result. It behaves the same as a normal GenericRestReply, however,
//! there is no DataClassType, just void, for cases where you don't care about the result itself, only the code.
//! @copydoc QtRestClient::GenericRestReply
template <typename ErrorClassType>
class GenericRestReply<void, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<ErrorClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded
	GenericRestReply<void, ErrorClassType> *onSucceeded(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onFailed
	GenericRestReply<void, ErrorClassType> *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<void, ErrorClassType> *onSerializeException(std::function<void(QJsonSerializerException &)> handler);
	//! @copydoc GenericRestReply::onAllErrors
	GenericRestReply<void, ErrorClassType> *onAllErrors(std::function<void(QString, int, ErrorType)> handler,
																		std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc GenericRestReply::onCompleted
	GenericRestReply<void, ErrorClassType> *onCompleted(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onError
	GenericRestReply<void, ErrorClassType> *onError(std::function<void(QString, int, ErrorType)> handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<void, ErrorClassType> *disableAutoDelete();

private:
	RestClient *client;
	std::function<void(QJsonSerializerException &)> exceptionHandler;
};

//! @note This class is a simple specialization for list types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create requests and replies with lists (using QList<>). Of cause, the DataClassType, as generic parameter
//! for the QList, must fullfill the same restrictions as for the normal one
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<DataClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(int, QList<DataClassType>)> handler);
	//! @copydoc GenericRestReply::onFailed
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(QJsonSerializerException &)> handler);
	//! @copydoc GenericRestReply::onAllErrors
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onAllErrors(std::function<void(QString, int, ErrorType)> handler,
																		std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	//! @copydoc GenericRestReply::onCompleted
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onCompleted(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onError
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onError(std::function<void(QString, int, ErrorType)> handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<QList<DataClassType>, ErrorClassType> *disableAutoDelete();

private:
	RestClient *client;
	std::function<void(QJsonSerializerException &)> exceptionHandler;
};

//! @note This class is a simple specialization for paging types. It behaves the same as a normal GenericRestReply, however,
//! it allows you to create replies with paging logic (using Paging<>). Of cause, the DataClassType, as generic parameter
//! for the Paging, must fullfill the same restrictions as for the normal one. Paging allows you to only get a part of a
//! list form the server, and allows you to iterate over those results.
//! @copydoc QtRestClient::GenericRestReply
template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public RestReply
{
	static_assert(MetaComponent<DataClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
	static_assert(MetaComponent<ErrorClassType>::is_meta::value, "DataClassType must inherit QObject or have Q_GADGET!");
public:
	//! @copydoc GenericRestReply::GenericRestReply
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	//! @copydoc GenericRestReply::onSucceeded
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(int, Paging<DataClassType>)> handler);
	//! @copydoc GenericRestReply::onFailed
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(std::function<void(int, ErrorClassType)> handler);
	//! @copydoc GenericRestReply::onSerializeException
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(QJsonSerializerException &)> handler);
	//! @copydoc GenericRestReply::onAllErrors
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onAllErrors(std::function<void(QString, int, ErrorType)> handler,
																		 std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//! shortcut to iterate over all elements via paging objects
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(std::function<bool(DataClassType, int)> iterator, int to = -1, int from = 0);

	//overshadowing, for the right return type only..
	//! @copydoc GenericRestReply::onCompleted
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onCompleted(std::function<void(int)> handler);
	//! @copydoc GenericRestReply::onError
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onError(std::function<void(QString, int, ErrorType)> handler);
	//! @copydoc GenericRestReply::disableAutoDelete
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *disableAutoDelete();

private:
	RestClient *client;
	std::function<void(int, ErrorClassType)> failureHandler;
	std::function<void(QString, int, ErrorType)> errorHandler;
	std::function<void(QJsonSerializerException &)> exceptionHandler;
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
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<void (int, DataClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<DataClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSerializeException(std::function<void (QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onAllErrors(std::function<void (QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](QJsonSerializerException exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onError(std::function<void (QString, int, RestReply::ErrorType)> handler)
{
	RestReply::onError(handler);
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
{}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSucceeded(std::function<void (int)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &){
		handler(code);
	});
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onSerializeException(std::function<void (QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onAllErrors(std::function<void (QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](QJsonSerializerException exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename ErrorClassType>
GenericRestReply<void, ErrorClassType> *GenericRestReply<void, ErrorClassType>::onError(std::function<void (QString, int, RestReply::ErrorType)> handler)
{
	RestReply::onError(handler);
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
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (int, QList<DataClassType>)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isArray())
				throw QJsonDeserializationException("Expected JSON array but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<DataClassType>(value.toArray()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSerializeException(std::function<void (QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onAllErrors(std::function<void (QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](QJsonSerializerException exception){
		handler(QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onError(std::function<void (QString, int, RestReply::ErrorType)> handler)
{
	RestReply::onError(handler);
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (int, Paging<DataClassType>)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			auto iPaging = client->pagingFactory()->createPaging(client->serializer(), value.toObject());
			auto data = client->serializer()->deserialize<DataClassType>(iPaging->items());
			handler(code, Paging<DataClassType>(iPaging, data, client));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onFailed(std::function<void (int, ErrorClassType)> handler)
{
	failureHandler = handler;
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + jsonTypeName(value.type()));
			handler(code, client->serializer()->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(e);
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSerializeException(std::function<void (QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onAllErrors(std::function<void (QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](int code, ErrorClassType obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
		MetaComponent<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](QJsonSerializerException exception){
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(std::function<bool (DataClassType, int)> iterator, int to, int from)
{
	return onSucceeded([=](int, Paging<DataClassType> paging){
		paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, from);
	});
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onCompleted(std::function<void (int)> handler)
{
	RestReply::onCompleted(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onError(std::function<void (QString, int, RestReply::ErrorType)> handler)
{
	errorHandler = handler;
	RestReply::onError(handler);
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
