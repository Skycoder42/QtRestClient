#include "restclass.h"
#include "restclass_p.h"
#include "restclient.h"
using namespace QtRestClient;

#define d d_ptr

const QByteArray RestClass::GetVerb("GET");
const QByteArray RestClass::PostVerb("POST");
const QByteArray RestClass::PutVerb("PUT");
const QByteArray RestClass::DeleteVerb("DELETE");
const QByteArray RestClass::PatchVerb("PATCH");

RestClass::RestClass(RestClient *client, QStringList subPath, QObject *parent) :
	QObject(parent),
	d_ptr(new RestClassPrivate(client, subPath))
{
	connect(client, &RestClient::destroyed,
			this, &RestClass::deleteLater);
}

RestClass::~RestClass() {}

RestClass *RestClass::subClass(const QString &path, QObject *parent)
{
	auto nPath = d->subPath;
	nPath.append(path.split(QLatin1Char('/'), QString::SkipEmptyParts));
	return new RestClass(d->client, nPath, parent);
}

RestReply *RestClass::call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(builder()
						 .addPath(methodPath)
						 .addParameters(RestClassPrivate::hashToQuery(parameters))
						 .addHeaders(headers)
						 .setVerb(verb)
						 .send(),
						 this);
}

RestReply *RestClass::call(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(builder()
						 .addPath(methodPath)
						 .addParameters(RestClassPrivate::hashToQuery(parameters))
						 .addHeaders(headers)
						 .setBody(body)
						 .setVerb(verb)
						 .send(),
						 this);
}

RestReply *RestClass::call(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(builder()
						 .addPath(methodPath)
						 .addParameters(RestClassPrivate::hashToQuery(parameters))
						 .addHeaders(headers)
						 .setBody(body)
						 .setVerb(verb)
						 .send(),
						 this);
}

RequestBuilder RestClass::builder() const
{
	return d->client->builder()
			.addPath(d->subPath);
}

// ------------- Private Implementation -------------

QUrlQuery RestClassPrivate::hashToQuery(const QVariantHash &hash)
{
	QUrlQuery query;
	for(auto it = hash.constBegin(); it != hash.constEnd(); it++)
		query.addQueryItem(it.key(), it.value().toString());
	return query;
}

RestClassPrivate::RestClassPrivate(RestClient *client, QStringList subPath) :
	client(client),
	subPath(subPath)
{}
