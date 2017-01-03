#ifndef GENERICRESTREPLY_H
#define GENERICRESTREPLY_H

#include "jsonserializer.h"
#include "restobject.h"
#include "restreply.h"

namespace QtRestClient {

template <typename DataClassType, typename ErrorClassType = RestObject>
class GenericRestReply : public RestReply
{
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<DataClassType, ErrorClassType> &onSucceeded(std::function<bool(RestReply*, int, DataClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onFailed(std::function<bool(RestReply*, int, ErrorClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onSerializeException(std::function<void(RestReply*, SerializerException &)> handler);

private:
	JsonSerializer *serializer;
	std::function<void(RestReply*, SerializerException &)> exceptionHandler;
};

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<QList<DataClassType>, ErrorClassType> &onSucceeded(std::function<bool(RestReply*, int, QList<DataClassType*>)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> &onFailed(std::function<bool(RestReply*, int, ErrorClassType*)> handler);
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
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<bool (RestReply *, int, DataClassType *)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		DataClassType *ptr = nullptr;
		try {
			ptr = serializer->deserialize<DataClassType>(value.toObject());
			if(handler(this, code, ptr))
				ptr = nullptr;
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
		if(ptr)
			ptr->deleteLater();
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onFailed(std::function<bool (RestReply *, int, ErrorClassType *)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		ErrorClassType *ptr = nullptr;
		try {
			ptr = serializer->deserialize<ErrorClassType>(value.toObject());
			if(handler(this, code, ptr))
				ptr = nullptr;
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
		if(ptr)
			ptr->deleteLater();
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
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(std::function<bool (RestReply *, int, QList<DataClassType*>)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		QList<DataClassType*> ptrLst;
		try {
			ptrLst = serializer->deserialize<DataClassType>(value.toArray());
			if(handler(this, code, ptrLst))
				ptrLst.clear();
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
		foreach(auto obj, ptrLst)
			obj->deleteLater();
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(std::function<bool (RestReply *, int, ErrorClassType *)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		ErrorClassType *ptr = nullptr;
		try {
			ptr = serializer->deserialize<ErrorClassType>(value.toObject());
			if(handler(this, code, ptr))
				ptr = nullptr;
		} catch(SerializerException &e) {
			if(exceptionHandler)
				exceptionHandler(this, e);
			else
				throw;
		}
		if(ptr)
			ptr->deleteLater();
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
