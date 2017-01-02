#ifndef RESTCLASS_H
#define RESTCLASS_H

#include "qtrestclient_global.h"
#include "requestbuilder.h"
#include "restreply.h"
#include "genericrestreply.h"
#include "restclient.h"

#include <QObject>

namespace QtRestClient {

class RestClassPrivate;
class QTRESTCLIENTSHARED_EXPORT RestClass : public QObject
{
	Q_OBJECT
	friend class RestClient;
	friend class RestClientPrivate;

public:
	static const QByteArray GetVerb;
	static const QByteArray PostVerb;
	static const QByteArray PutVerb;
	static const QByteArray DeleteVerb;
	static const QByteArray PatchVerb;

	~RestClass();

	RestClient *client() const;
	RestClass *subClass(const QString &path, QObject *parent = nullptr);

	//general calls
	RestReply *call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *call(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *call(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename BT, typename DT, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, BT *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT, typename ET = RestObject, typename... Args>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, Args... parameters);
	template<typename BT, typename DT, typename ET = RestObject, typename... Args>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, BT *body, Args... parameters);
	template<typename DT, typename ET = RestObject, typename... Args>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, const HeaderHash & headers, Args... parameters);
	template<typename BT, typename DT, typename ET = RestObject, typename... Args>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, BT *body, const HeaderHash & headers, Args... parameters);

	RequestBuilder builder() const;

private:
	QScopedPointer<RestClassPrivate> d_ptr;

	explicit RestClass(RestClient *client, QStringList subPath, QObject *parent);

	QNetworkReply *create(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash & headers);

	QVariantHash concatParameters(QString key, QVariant value);
	template<typename... Args>
	QVariantHash concatParameters(QString key, QVariant value, Args... parameters);
};

// ------------- Generic Implementation -------------

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   parameters,
											   headers),
										client(),
										this);
}

template<typename BT, typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, BT *body, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										this->client(),
										this);
}

template<typename DT, typename ET, typename... Args>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, Args... parameters)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   concatParameters(parameters),
											   {}),
										client(),
										this);
}

template<typename BT, typename DT, typename ET, typename... Args>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, BT *body, Args... parameters)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   concatParameters(parameters),
											   {}),
										this->client(),
										this);
}

template<typename DT, typename ET, typename... Args>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, const HeaderHash & headers, Args... parameters)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   concatParameters(parameters),
											   headers),
										client(),
										this);
}

template<typename BT, typename DT, typename ET, typename... Args>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, BT *body, const HeaderHash & headers, Args... parameters)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   concatParameters(parameters),
											   headers),
										this->client(),
										this);
}

}

#endif // RESTCLASS_H
