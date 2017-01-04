#ifndef GENERICRESTREPLY_H
#define GENERICRESTREPLY_H

#include "jsonserializer.h"
#include "restobject.h"
#include "restreply.h"

#include <type_traits>

namespace QtRestClient {

template <typename DataClassType, typename ErrorClassType = RestObject>
class GenericRestReply : public RestReply
{
	static_assert(std::is_base_of<RestObject, DataClassType>::value, "DataClassType must inherit RestObject!");
	static_assert(std::is_base_of<RestObject, ErrorClassType>::value, "ErrorClassType must inherit RestObject!");
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<DataClassType, ErrorClassType> &onSucceeded(std::function<void(RestReply*, int, DataClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onFailed(std::function<void(RestReply*, int, ErrorClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onSerializeException(std::function<void(RestReply*, SerializerException &)> handler);

private:
	JsonSerializer *serializer;
	std::function<void(RestReply*, SerializerException &)> exceptionHandler;
};

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
	static_assert(std::is_base_of<RestObject, DataClassType>::value, "DataClassType must inherit RestObject!");
	static_assert(std::is_base_of<RestObject, ErrorClassType>::value, "ErrorClassType must inherit RestObject!");
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<QList<DataClassType>, ErrorClassType> &onSucceeded(std::function<void(RestReply*, int, QList<DataClassType*>)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> &onFailed(std::function<void(RestReply*, int, ErrorClassType*)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> &onSerializeException(std::function<void(RestReply*, SerializerException &)> handler);

private:
	JsonSerializer *serializer;
	std::function<void(RestReply*, SerializerException &)> exceptionHandler;
};

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, JsonSerializer *serializer, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(serializer),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<void (RestReply *, int, DataClassType *)> handler)
{
	if(!handler)
		return *this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		DataClassType *ptr = nullptr;
		try {
			if(!value.isObject())
				throw SerializerException(QStringLiteral("Expected JSON object but got %1").arg(value.type()), true);
			ptr = serializer->deserialize<DataClassType>(value.toObject());
			handler(this, code, ptr);
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onFailed(std::function<void (RestReply *, int, ErrorClassType *)> handler)
{
	if(!handler)
		return *this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		ErrorClassType *ptr = nullptr;
		try {
			if(!value.isObject())
				throw SerializerException(QStringLiteral("Expected JSON object but got %1").arg(value.type()), true);
			ptr = serializer->deserialize<ErrorClassType>(value.toObject());
			handler(this, code, ptr);
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onSerializeException(std::function<void (RestReply *, SerializerException &)> handler)
{
	exceptionHandler = handler;
	return *this;
}

// ------------- Implementation List of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, JsonSerializer *serializer, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(serializer),
	exceptionHandler()
{}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(std::function<void (RestReply *, int, QList<DataClassType*>)> handler)
{
	if(!handler)
		return *this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		QList<DataClassType*> ptrLst;
		try {
			if(!value.isArray())
				throw SerializerException(QStringLiteral("Expected JSON object but got %1").arg(value.type()), true);
			ptrLst = serializer->deserialize<DataClassType>(value.toArray());
			handler(this, code, ptrLst);
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(std::function<void (RestReply *, int, ErrorClassType *)> handler)
{
	if(!handler)
		return *this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		ErrorClassType *ptr = nullptr;
		try {
			if(!value.isObject())
				throw SerializerException(QStringLiteral("Expected JSON object but got %1").arg(value.type()), true);
			ptr = serializer->deserialize<ErrorClassType>(value.toObject());
			handler(this, code, ptr);
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onSerializeException(std::function<void (RestReply *, SerializerException &)> handler)
{
	exceptionHandler = handler;
	return *this;
}

}

#endif // GENERICRESTREPLY_H
