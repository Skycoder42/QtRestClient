#include "jsonserializer.h"

#include <QJsonArray>
using namespace QtRestClient;

namespace QtRestClient {
class JsonSerializerPrivate
{
public:
	QString errorString;

	JsonSerializerPrivate(JsonSerializer *q_ptr);

	QJsonValue doSerialize(const QMetaProperty &property, const QVariant &value, bool *ok);
	QJsonObject serializeObject(const RestObject *restObject, bool *ok);

private:
	JsonSerializer *q_ptr;
};
}

#define d d_ptr

JsonSerializer::JsonSerializer(QObject *parent) :
	QObject(parent),
	d_ptr(new JsonSerializerPrivate(this))
{}

JsonSerializer::~JsonSerializer() {}

QJsonObject JsonSerializer::serialize(const RestObject *restObject, bool *ok) const
{
	return d->serializeObject(restObject, ok);
}

RestObject *JsonSerializer::deserialize(QJsonObject jsonObject, QMetaObject *metaObject, QObject *parent) const
{
	return nullptr;
}

QString JsonSerializer::errorString() const
{
	return d->errorString;
}

QJsonValue JsonSerializer::serializeValue(QVariant value, bool *ok)
{
	if(value.isNull())
		return QJsonValue::Null;
	else {
		auto json = QJsonValue::fromVariant(value);
		if(ok)
			*ok = !json.isNull();
		return json;
	}
}

QVariant JsonSerializer::deserializeValue(QJsonValue value, QMetaProperty *metaProperty)
{
	return QVariant();
}

// ------------- Private Implementation -------------

JsonSerializerPrivate::JsonSerializerPrivate(JsonSerializer *q_ptr) :
	q_ptr(q_ptr)
{}

QJsonValue JsonSerializerPrivate::doSerialize(const QMetaProperty &property, const QVariant &value, bool *ok)
{
	auto lValue = value;
	if((property.isValid() && property.type() == QVariant::List) ||
	   (lValue.canConvert(QVariant::List) && lValue.convert(QVariant::List))) {
		QJsonArray array;
		foreach(auto element, lValue.toList()) {
			array.append(doSerialize({}, element, ok));
			if(!ok)
				return {};
		}
		return array;
	} else {
		lValue = value;
		auto restId = qMetaTypeId<RestObject*>();
		if(lValue.canConvert(restId) && lValue.convert(restId)) {
			auto restObject = lValue.value<RestObject*>();
			if(restObject)
				return serializeObject(restObject, ok);
			else
				return QJsonValue::Null;
		} else
			return q_ptr->serializeValue(value, ok);
	}
}

QJsonObject JsonSerializerPrivate::serializeObject(const RestObject *restObject, bool *ok)
{
	auto meta = restObject->metaObject();

	QJsonObject object;
	//go through all properties and try to serialize them
	for(auto i = RestObject::staticMetaObject.propertyOffset(); i !=meta->propertyCount(); i++) {
		auto property = meta->property(i);
		object[property.name()]= doSerialize(property, property.read(restObject), ok);
		if(!ok)
			return {};
	}

	return object;
}
