#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include "qtrestclient_global.h"
#include "restobject.h"
#include "restexception.h"

#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>

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

public:
	explicit JsonSerializer(QObject *parent = nullptr);
	~JsonSerializer();

	virtual QJsonObject serialize(const RestObject *restObject) const;
	virtual RestObject *deserialize(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent = nullptr) const;

protected:
	virtual QJsonValue serializeValue(QVariant value);
	virtual QVariant deserializeValue(QJsonValue value);

private:
	QScopedPointer<JsonSerializerPrivate> d_ptr;
};

}

#endif // JSONSERIALIZER_H
