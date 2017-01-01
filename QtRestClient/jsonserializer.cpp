#include "jsonserializer.h"

#include <QJsonArray>
using namespace QtRestClient;

namespace QtRestClient {
class JsonSerializerPrivate
{
public:
	JsonSerializerPrivate(JsonSerializer *q_ptr);

	QJsonValue doSerialize(const QMetaProperty &property, const QVariant &value);
	QJsonObject serializeObject(const RestObject *restObject);

	QVariant doDeserialize(const QMetaProperty &property, const QJsonValue &value);
	RestObject *deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent);

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

QJsonObject JsonSerializer::serialize(const RestObject *restObject) const
{
	return d->serializeObject(restObject);
}

RestObject *JsonSerializer::deserialize(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const
{
	return d->deserializeObject(jsonObject, metaObject, parent);
}

QJsonValue JsonSerializer::serializeValue(QVariant value)
{
	if(value.isNull())
		return QJsonValue::Null;
	else {
		auto json = QJsonValue::fromVariant(value);
		if(json.isNull())
			throw SerializerException(QStringLiteral("Failed to convert type %1 to a JSON representation").arg(value.typeName()), false);
		else
			return json;
	}
}

QVariant JsonSerializer::deserializeValue(QJsonValue value)
{
	return QVariant();
}

// ------------- Private Implementation -------------

JsonSerializerPrivate::JsonSerializerPrivate(JsonSerializer *q_ptr) :
	q_ptr(q_ptr)
{}

QJsonValue JsonSerializerPrivate::doSerialize(const QMetaProperty &property, const QVariant &value)
{
	auto lValue = value;
	if((property.isValid() && property.type() == QVariant::List) ||
	   (lValue.canConvert(QVariant::List) && lValue.convert(QVariant::List))) {
		QJsonArray array;
		foreach(auto element, lValue.toList())
			array.append(doSerialize({}, element));
		return array;
	} else {
		lValue = value;
		auto restId = qMetaTypeId<RestObject*>();
		if(lValue.canConvert(restId) && lValue.convert(restId)) {
			auto restObject = lValue.value<RestObject*>();
			if(restObject)
				return serializeObject(restObject);
			else
				return QJsonValue::Null;
		} else
			return q_ptr->serializeValue(value);
	}
}

QJsonObject JsonSerializerPrivate::serializeObject(const RestObject *restObject)
{
	auto meta = restObject->metaObject();

	QJsonObject object;
	//go through all properties and try to serialize them
	for(auto i = RestObject::staticMetaObject.propertyOffset(); i !=meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if(property.isStored())
			object[property.name()]= doSerialize(property, property.read(restObject));
	}

	return object;
}

QVariant JsonSerializerPrivate::doDeserialize(const QMetaProperty &property, const QJsonValue &value)
{
	QVariant variant;
	if(value.isArray()) {
		QVariantList vList;
		foreach(auto element, value.toArray())
			vList.append(doDeserialize({}, element));
		variant = vList;
	} else if(value.isObject()) {
		//SHIT
	} else
		variant = q_ptr->deserializeValue(value);

	if(property.isValid()) {
		auto vType = variant.typeName();
		if(variant.canConvert(property.userType()) && variant.convert(property.userType()))
			return variant;
		else {
			throw SerializerException(QStringLiteral("Failed to convert deserialized variant of type %1 to property type %2")
									  .arg(vType)
									  .arg(property.typeName()),
									  true);
		}
	} else
		return variant;
}

RestObject *JsonSerializerPrivate::deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent)
{
	//try to construct the object
	auto object = qobject_cast<RestObject*>(metaObject->newInstance(Q_ARG(QObject*, parent)));
	if(!object)
		throw SerializerException(QStringLiteral("Failed to construct object of type %1").arg(metaObject->className()), true);

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		object->setProperty(qUtf8Printable(it.key()),
							doDeserialize(metaObject->property(propIndex), it.value()));
	}

	return object;
}

// ------------- SerializerException Implementation -------------

SerializerException::SerializerException(QString what, bool deser) :
	Exception(QStringLiteral("Failed to %1 with error: %2")
			  .arg(deser ? "deserialize" : "serialize")
			  .arg(what))
{}
