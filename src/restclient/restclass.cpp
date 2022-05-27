#include "restclass.h"
#include "restclass_p.h"
#include "restclient.h"
using namespace QtRestClient;

const QByteArray RestClass::GetVerb("GET");
const QByteArray RestClass::PostVerb("POST");
const QByteArray RestClass::PutVerb("PUT");
const QByteArray RestClass::DeleteVerb("DELETE");
const QByteArray RestClass::PatchVerb("PATCH");
const QByteArray RestClass::HeadVerb("HEAD");

RestClass::RestClass(RestClient *client, QStringList subPath, QObject *parent) :
	  RestClass{*new RestClassPrivate{}, parent}
{
	Q_D(RestClass);
	d->client = client;
	d->subPath = std::move(subPath);
	connect(client, &RestClient::destroyed,
			this, &RestClass::deleteLater);
}

RestClass::RestClass(RestClassPrivate &dd, QObject *parent) :
	  QObject{dd, parent}
{}

RestClient *RestClass::client() const
{
	Q_D(const RestClass);
	return d->client;
}

RestClass *RestClass::subClass(const QString &path, QObject *parent) const
{
	Q_D(const RestClass);
	auto nPath = d->subPath;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	nPath.append(path.split(QLatin1Char('/'), QString::SkipEmptyParts));
#else
	nPath.append(path.split(QLatin1Char('/'), Qt::SkipEmptyParts));
#endif
	return new RestClass{d->client, std::move(nPath), parent};
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, methodPath, parameters, headers, paramsAsBody));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QString &methodPath, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, methodPath, body, parameters, headers));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QString &methodPath, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, methodPath, body, parameters, headers));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, parameters, headers, paramsAsBody));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, body, parameters, headers));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, body, parameters, headers));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, relativeUrl, parameters, headers, paramsAsBody));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QUrl &relativeUrl, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, relativeUrl, body, parameters, headers));
}

RestReply *RestClass::callRaw(const QByteArray &verb, const QUrl &relativeUrl, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	return std::visit([&](const auto &reply) {
#ifdef QT_RESTCLIENT_USE_ASYNC
		Q_D(const RestClass);
		return new RestReply{reply, d->client->asyncPool(), nullptr};
#else
		return new RestReply{reply, nullptr};
#endif
	}, create(verb, relativeUrl, body, parameters, headers));
}

RequestBuilder RestClass::builder() const
{
	Q_D(const RestClass);
	return d->client->builder()
			.addPath(d->subPath);
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	auto cBuilder = (paramsAsBody ?
				builder().addPostParameters(RestClassPrivate::hashToQuery(parameters)) :
				builder().addParameters(RestClassPrivate::hashToQuery(parameters)))
		.addPath(methodPath)
		.addHeaders(headers)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QString &methodPath, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.addPath(methodPath)
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QString &methodPath, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.addPath(methodPath)
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	auto cBuilder = (paramsAsBody ?
				builder().addPostParameters(RestClassPrivate::hashToQuery(parameters)) :
				builder().addParameters(RestClassPrivate::hashToQuery(parameters)))
		.addHeaders(headers)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	auto cBuilder = (paramsAsBody ?
				builder().addPostParameters(RestClassPrivate::hashToQuery(parameters)) :
				builder().addParameters(RestClassPrivate::hashToQuery(parameters)))
		.updateFromRelativeUrl(relativeUrl, true)
		.addHeaders(headers)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QUrl &relativeUrl, const QCborValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.updateFromRelativeUrl(relativeUrl, true)
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

RestClass::CreateResult RestClass::create(const QByteArray &verb, const QUrl &relativeUrl, const QJsonValue &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	auto cBuilder = builder()
		.updateFromRelativeUrl(relativeUrl, true)
		.addParameters(RestClassPrivate::hashToQuery(parameters))
		.addHeaders(headers)
		.setBody(body, false)
		.setVerb(verb);
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (client()->isThreaded())
		return cBuilder.sendAsync();
	else
#endif
		return cBuilder.send();
}

// ------------- Private Implementation -------------

QUrlQuery RestClassPrivate::hashToQuery(const QVariantHash &hash)
{
	QUrlQuery query;
	for (auto it = hash.constBegin(); it != hash.constEnd(); it++)
		query.addQueryItem(it.key(), it.value().toString());
	return query;
}
