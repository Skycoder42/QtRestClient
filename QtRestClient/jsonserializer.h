#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include "qtrestclient_global.h"
#include "restobject.h"
#include "restexception.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaProperty>
#include <QObject>
#include <type_traits>

namespace QtRestClient {

class QTRESTCLIENTSHARED_EXPORT SerializerException : public Exception
{
public:
	SerializerException(QString what, bool deser);
};

class JsonSerializerPrivate;
class QTRESTCLIENTSHARED_EXPORT JsonSerializer : public QObject
{
	Q_OBJECT
	friend class JsonSerializerPrivate;

	Q_PROPERTY(bool allowDefaultNull READ allowDefaultNull WRITE setAllowDefaultNull)

public:
	explicit JsonSerializer(QObject *parent = nullptr);
	~JsonSerializer();

	bool allowDefaultNull() const;

	virtual QJsonObject serialize(const RestObject *restObject) const;
	template<typename T>
	QJsonArray serialize(const QList<T*> restObjects) const;

	virtual RestObject *deserialize(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	T *deserialize(QJsonObject jsonObject, QObject *parent = nullptr) const;
	QList<RestObject*> deserialize(QJsonArray jsonArray, const QMetaObject *metaObject, QObject *parent = nullptr) const;
	template<typename T>
	QList<T*> deserialize(QJsonArray jsonArray, QObject *parent = nullptr) const;

public slots:
	void setAllowDefaultNull(bool allowDefaultNull);

protected:
	virtual QJsonValue serializeValue(QVariant value);
	virtual QVariant deserializeValue(QJsonValue value);

private:
	QScopedPointer<JsonSerializerPrivate> d_ptr;
};

// ------------- Generic Implementation -------------

template<typename T>
QJsonArray JsonSerializer::serialize(const QList<T*> restObjects) const
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
	QJsonArray array;
	foreach(auto obj, restObjects)
		array.append(serialize(obj));
	return array;
}

template<typename T>
T *JsonSerializer::deserialize(QJsonObject jsonObject, QObject *parent) const
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
	return static_cast<T*>(deserialize(jsonObject, &T::staticMetaObject, parent));
}

template<typename T>
QList<T*> JsonSerializer::deserialize(QJsonArray jsonArray, QObject *parent) const
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
	QList<T*> list;
	foreach(auto json, jsonArray) {
		if(json.isObject())
			list.append(deserialize<T>(json.toObject(), parent));
		else {
			throw SerializerException(QStringLiteral("Failed convert array element of type %1 to %2")
									  .arg(json.type())
									  .arg(T::staticMetaObject.className()),
									  true);
		}
	}
	return list;
}

}

#endif // JSONSERIALIZER_H
