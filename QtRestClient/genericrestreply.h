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

	GenericRestReply<DataClassType, ErrorClassType> &onSucceeded(std::function<bool(GenericRestReply<DataClassType, ErrorClassType>*, int, DataClassType*)> handler);
	GenericRestReply<DataClassType, ErrorClassType> &onFailed(std::function<bool(GenericRestReply<DataClassType, ErrorClassType>*, int, ErrorClassType*)> handler);

private:
	JsonSerializer *serializer;
};

// ------------- Implementation -------------

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

}

#endif // GENERICRESTREPLY_H
