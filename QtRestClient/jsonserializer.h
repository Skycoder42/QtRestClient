#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include "qtrestclient_global.h"
#include "restobject.h"

#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>

namespace QtRestClient {

class JsonSerializerPrivate;
class QTRESTCLIENTSHARED_EXPORT JsonSerializer : public QObject
{
	Q_OBJECT
	friend class JsonSerializerPrivate;

	Q_PROPERTY(QString errorString READ errorString)

public:
	explicit JsonSerializer(QObject *parent = nullptr);
	~JsonSerializer();

	virtual QJsonObject serialize(const RestObject *restObject, bool *ok) const;
	virtual RestObject *deserialize(QJsonObject jsonObject, QMetaObject *metaObject, QObject *parent = nullptr) const;

	QString errorString() const;

protected:
	virtual QJsonValue serializeValue(QVariant value, bool *ok);
	virtual QVariant deserializeValue(QJsonValue value, QMetaProperty *metaProperty);

private:
	QScopedPointer<JsonSerializerPrivate> d_ptr;
};

}

#endif // JSONSERIALIZER_H
