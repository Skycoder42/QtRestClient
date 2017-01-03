#ifndef GENERICRESTREPLY_H
#define GENERICRESTREPLY_H

#include "jsonserializer.h"
#include "restobject.h"
#include "restreply.h"

namespace QtRestClient {

//TODO serializer exception handling!
//TODO qobject_cast --> static_cast!
//TODO "rep" parameter, use base class instead?
template <typename DataClassType, typename ErrorClassType = RestObject>
class GenericRestReply : public RestReply
{
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<DataClassType, ErrorClassType> &onSucceeded(std::function<bool(GenericRestReply<DataClassType, ErrorClassType>*, int, DataClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onFailed(std::function<bool(GenericRestReply<DataClassType, ErrorClassType>*, int, ErrorClassType*)> handler);

private:
	JsonSerializer *serializer;
};

template <typename DataClassType, typename ErrorClassType>
class GenericRestReply<QList<DataClassType>, ErrorClassType> : public RestReply
{
public:
	GenericRestReply(QNetworkReply *networkReply,
					 JsonSerializer *serializer,
					 QObject *parent = nullptr);

	GenericRestReply<QList<DataClassType>, ErrorClassType> &onSucceeded(std::function<bool(GenericRestReply<QList<DataClassType>, ErrorClassType>*, int, QList<DataClassType*>)> handler);
	GenericRestReply<QList<DataClassType>, ErrorClassType> &onFailed(std::function<bool(GenericRestReply<QList<DataClassType>, ErrorClassType>*, int, ErrorClassType*)> handler);

private:
	JsonSerializer *serializer;

	static QList<DataClassType*> list_cast(const QList<RestObject*> &lst);
};

// ------------- Implementation Single Element -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<DataClassType, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, JsonSerializer *serializer, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(serializer)
{}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onSucceeded(std::function<bool (GenericRestReply<DataClassType, ErrorClassType> *, int, DataClassType *)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		RestObject *ptr = serializer->deserialize(value.toObject(), &DataClassType::staticMetaObject);
		if(!handler(this, code, qobject_cast<DataClassType*>(ptr))) {
			if(ptr)
				ptr->deleteLater();
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<DataClassType, ErrorClassType> &GenericRestReply<DataClassType, ErrorClassType>::onFailed(std::function<bool (GenericRestReply<DataClassType, ErrorClassType> *, int, ErrorClassType *)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		RestObject *ptr = serializer->deserialize(value.toObject(), &ErrorClassType::staticMetaObject);
		if(!handler(this, code, qobject_cast<ErrorClassType*>(ptr))) {
			if(ptr)
				ptr->deleteLater();
		}
	});
	return *this;
}

// ------------- Implementation List of Elements -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReply<QList<DataClassType>, ErrorClassType>::GenericRestReply(QNetworkReply *networkReply, JsonSerializer *serializer, QObject *parent) :
	RestReply(networkReply, parent),
	serializer(serializer)
{}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onSucceeded(std::function<bool (GenericRestReply<QList<DataClassType>, ErrorClassType> *, int, QList<DataClassType*>)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		QList<RestObject*> ptrLst = serializer->deserialize(value.toArray(), &DataClassType::staticMetaObject);
		if(!handler(this, code, list_cast(ptrLst)))
			qDeleteAll(ptrLst);//TODO delete all later
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReply<QList<DataClassType>, ErrorClassType> &GenericRestReply<QList<DataClassType>, ErrorClassType>::onFailed(std::function<bool (GenericRestReply<QList<DataClassType>, ErrorClassType> *, int, ErrorClassType *)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		RestObject *ptr = serializer->deserialize(value.toObject(), &ErrorClassType::staticMetaObject);
		if(!handler(this, code, qobject_cast<ErrorClassType*>(ptr))) {
			if(ptr)
				ptr->deleteLater();
		}
	});
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
QList<DataClassType*> GenericRestReply<QList<DataClassType>, ErrorClassType>::list_cast(const QList<RestObject *> &lst)
{
	QList<DataClassType*> resLst;
	foreach(auto obj, lst)
		resLst.append(qobject_cast<DataClassType*>(obj));
	return resLst;
}

}

#endif // GENERICRESTREPLY_H
