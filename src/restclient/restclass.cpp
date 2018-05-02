#include "restclass.h"
#include "restclass_p.h"
#include "restclient.h"
using namespace QtRestClient;

const QByteArray RestClass::GetVerb("GET");
const QByteArray RestClass::PostVerb("POST");
const QByteArray RestClass::PutVerb("PUT");
const QByteArray RestClass::DeleteVerb("DELETE");
const QByteArray RestClass::PatchVerb("PATCH");

RestClass::RestClass(RestClient *client, QStringList subPath, QObject *parent) :
	QObject(parent),
	d(new RestClassPrivate(client, subPath))
{
	connect(client, &RestClient::destroyed,
			this, &RestClass::deleteLater);
}

RestClass::~RestClass() = default;

RestClient *RestClass::client() const
{
	return d->client;
}

RestClass *RestClass::subClass(const QString &path, QObject *parent)
{
	auto nPath = d->subPath;
	nPath.append(path.split(QLatin1Char('/'), QString::SkipEmptyParts));
	return new RestClass(d->client, nPath, parent);
}

RestReply *RestClass::callJson(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, methodPath, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, methodPath, body, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, methodPath, body, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, body, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, body, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, relativeUrl, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QUrl &relativeUrl, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, relativeUrl, body, parameters, headers), this);
}

RestReply *RestClass::callJson(QByteArray verb, const QUrl &relativeUrl, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new RestReply(create(verb, relativeUrl, body, parameters, headers), this);
}

RequestBuilder RestClass::builder() const
{
	return d->client->builder()
			.addPath(d->subPath);
}

QNetworkReply *RestClass::create(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addPath(methodPath)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addPath(methodPath)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addPath(methodPath)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.updateFromRelativeUrl(relativeUrl, true)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QUrl &relativeUrl, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.updateFromRelativeUrl(relativeUrl, true)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
}

QNetworkReply *RestClass::create(QByteArray verb, const QUrl &relativeUrl, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return builder()
			.updateFromRelativeUrl(relativeUrl, true)
			.addParameters(RestClassPrivate::hashToQuery(parameters))
			.addHeaders(headers)
			.setBody(body)
			.setVerb(verb)
			.send();
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
	subPath(std::move(subPath))
{}
