#ifndef GENERICRESTREPLY_H
#define GENERICRESTREPLY_H

#include "QtRestClient/qrestclient_global.h"
#include "QtRestClient/restclient.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/metadelete.h"

#include <QtJsonSerializer/qjsonserializer.h>
#include <type_traits>

namespace QtRestClient {

template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReply : public RestReply
{
	//TODO type assert
	//static_assert(std::is_base_of<QObject, DataClassType>::value, "DataClassType must inherit QObject!");
	//static_assert(std::is_base_of<QObject, ErrorClassType>::value, "ErrorClassType must inherit QObject!");
public:
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	GenericRestReply<DataClassType, ErrorClassType> *onSucceeded(std::function<void(RestReply*, int, DataClassType)> handler);
	GenericRestReply<DataClassType, ErrorClassType> *onFailed(std::function<void(RestReply*, int, ErrorClassType)> handler);
	GenericRestReply<DataClassType, ErrorClassType> *onSerializeException(std::function<void(RestReply*, QJsonSerializerException &)> handler);
	GenericRestReply<DataClassType, ErrorClassType> *onAllErrors(std::function<void(RestReply*, QString, int, ErrorType)> handler,
																 std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	inline GenericRestReply<DataClassType, ErrorClassType> *onError(std::function<void(RestReply*, QString, int, ErrorType)> handler);
	GenericRestReply<DataClassType, ErrorClassType> *enableAutoDelete();

private:
	QJsonSerializer *serializer;
	std::function<void(RestReply*, QJsonSerializerException &)> exceptionHandler;
};

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
	//TODO type assert
	//static_assert(std::is_base_of<QObject, DataClassType>::value, "DataClassType must inherit QObject!");
	//static_assert(std::is_base_of<QObject, ErrorClassType>::value, "ErrorClassType must inherit QObject!");
public:
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(RestReply*, int, QList<DataClassType>)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onFailed(std::function<void(RestReply*, int, ErrorClassType)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(RestReply*, QJsonSerializerException &)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onAllErrors(std::function<void(RestReply*, QString, int, ErrorType)> handler,
																		std::function<QString(ErrorClassType, int)> failureTransformer = {});

	//overshadowing, for the right return type only...
	GenericRestReply<QList<DataClassType>, ErrorClassType> *onError(std::function<void(RestReply*, QString, int, ErrorType)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> *enableAutoDelete();

private:
	QJsonSerializer *serializer;
	std::function<void(RestReply*, QJsonSerializerException &)> exceptionHandler;
};

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<Paging<DataClassType>, ErrorClassType> : public RestReply
{
	//TODO type assert
	//static_assert(std::is_base_of<QObject, DataClassType>::value, "DataClassType must inherit QObject!");
	//static_assert(std::is_base_of<QObject, ErrorClassType>::value, "ErrorClassType must inherit QObject!");
public:
	GenericRestReply(QNetworkReply *networkReply,
					 RestClient *client,
					 QObject *parent = nullptr);

	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSucceeded(std::function<void(RestReply*, int, Paging<DataClassType>)> handler);
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onFailed(std::function<void(RestReply*, int, ErrorClassType)> handler);
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onSerializeException(std::function<void(RestReply*, QJsonSerializerException &)> handler);
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onAllErrors(std::function<void(RestReply*, QString, int, ErrorType)> handler,
																		 std::function<QString(ErrorClassType, int)> failureTransformer = {});

	GenericRestReply<Paging<DataClassType>, ErrorClassType> *iterate(std::function<bool(Paging<DataClassType>*, DataClassType, int)> iterator, int to = -1, int from = 0);

	//overshadowing, for the right return type only...
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *onError(std::function<void(RestReply*, QString, int, ErrorType)> handler);
	GenericRestReply<Paging<DataClassType>, ErrorClassType> *enableAutoDelete();

private:
	RestClient *client;
	std::function<void(RestReply*, int, ErrorClassType)> failureHandler;
	std::function<void(RestReply*, QString, int, ErrorType)> errorHandler;
	std::function<void(RestReply*, QJsonSerializerException &)> exceptionHandler;
};

//include after delecation, to allow foreward declared types
#include "QtRestClient/paging.h"

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(client->serializer()),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<void (RestReply *, int, DataClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + QByteArray::number(value.type()));
			handler(this, code, serializer->deserialize<DataClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onFailed(std::function<void (RestReply *, int, ErrorClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + QByteArray::number(value.type()));
			handler(this, code, serializer->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onSerializeException(std::function<void (RestReply *, QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onAllErrors(std::function<void (RestReply *, QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](RestReply *rep, int code, ErrorClassType obj){
		if(failureTransformer)
			handler(rep, failureTransformer(obj, code), code, FailureError);
		else
			handler(rep, QString(), code, FailureError);
		MetaDelete<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](RestReply *rep, QJsonSerializerException exception){
		handler(rep, QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::onError(std::function<void (RestReply *, QString, int, RestReply::ErrorType)> handler)
{
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType> *GenericRestReply<DataClassType, ErrorClassType>::enableAutoDelete()
{
	RestReply::enableAutoDelete();
	return this;
}

// ------------- Implementation List of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, RestClient *client, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(client->serializer()),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (RestReply *, int, QList<DataClassType>)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isArray())
				throw QJsonDeserializationException("Expected JSON array but got " + QByteArray::number(value.type()));
			handler(this, code, serializer->deserialize<DataClassType>(value.toArray()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(std::function<void (RestReply *, int, ErrorClassType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + QByteArray::number(value.type()));
			handler(this, code, serializer->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onSerializeException(std::function<void (RestReply *, QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onAllErrors(std::function<void (RestReply *, QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](RestReply *rep, int code, ErrorClassType obj){
		if(failureTransformer)
			handler(rep, failureTransformer(obj, code), code, FailureError);
		else
			handler(rep, QString(), code, FailureError);
		MetaDelete<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](RestReply *rep, QJsonSerializerException exception){
		handler(rep, QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::onError(std::function<void (RestReply *, QString, int, RestReply::ErrorType)> handler)
{
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType> *GenericRestReply<QList<DataClassType>, ErrorClassType>::enableAutoDelete()
{
	RestReply::enableAutoDelete();
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
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (RestReply *, int, Paging<DataClassType>)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + QByteArray::number(value.type()));
			auto iPaging = client->pagingFactory()->createPaging(value.toObject());
			auto data = client->serializer()->deserialize<DataClassType>(iPaging->items());
			handler(this, code, Paging<DataClassType>(iPaging, data, client));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onFailed(std::function<void (RestReply *, int, ErrorClassType)> handler)
{
	failureHandler = handler;
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		try {
			if(!value.isObject())
				throw QJsonDeserializationException("Expected JSON object but got " + QByteArray::number(value.type()));
			handler(this, code, client->serializer()->deserialize<ErrorClassType>(value.toObject()));
		} catch(QJsonSerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onSerializeException(std::function<void (RestReply *, QJsonSerializerException &)> handler)
{
	exceptionHandler = handler;
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onAllErrors(std::function<void (RestReply *, QString, int, ErrorType)> handler, std::function<QString (ErrorClassType, int)> failureTransformer)
{
	this->onFailed([=](RestReply *rep, int code, ErrorClassType obj){
		if(failureTransformer)
			handler(rep, failureTransformer(obj, code), code, FailureError);
		else
			handler(rep, QString(), code, FailureError);
		MetaDelete<ErrorClassType>::deleteLater(obj);
	});
	this->onError(handler);
	this->onSerializeException([=](RestReply *rep, QJsonSerializerException exception){
		handler(rep, QString::fromUtf8(exception.what()), 0, DeserializationError);
	});
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::iterate(std::function<bool (Paging<DataClassType>*, DataClassType, int)> iterator, int to, int from)
{
	return onSucceeded([=](RestReply*, int, Paging<DataClassType> paging){
		paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, from);
	});
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::onError(std::function<void (RestReply *, QString, int, RestReply::ErrorType)> handler)
{
	errorHandler = handler;
	RestReply::onError(handler);
	return this;
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<Paging<DataClassType>, ErrorClassType> *GenericRestReply<Paging<DataClassType>, ErrorClassType>::enableAutoDelete()
{
	RestReply::enableAutoDelete();
	return this;
}

}

#endif // GENERICRESTREPLY_H
