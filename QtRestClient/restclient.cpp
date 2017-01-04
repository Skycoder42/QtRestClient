#include "restclient.h"
#include "restclient_p.h"
#include "restclass.h"
#include "standardpaging.h"
using namespace QtRestClient;

#define d d_ptr

RestClient::RestClient(QObject *parent) :
	QObject(parent),
	d_ptr(new RestClientPrivate(this))
{}

RestClient::~RestClient() {}

RestClass *RestClient::createClass(QString path, QObject *parent)
{
	return new RestClass(this, path.split(QLatin1Char('/'), QString::SkipEmptyParts), parent);
}

RestClass *RestClient::rootClass() const
{
	return d->rootClass;
}

JsonSerializer *RestClient::serializer() const
{
	return d->serializer;
}

PagingFactory *RestClient::pagingFactory() const
{
	return d->pagingFactory.data();
}

QUrl RestClient::baseUrl() const
{
	return d->baseUrl;
}

QVersionNumber RestClient::apiVersion() const
{
	return d->apiVersion;
}

HeaderHash RestClient::globalHeaders() const
{
	return d->headers;
}

QUrlQuery RestClient::globalParameters() const
{
	return d->query;
}

QSslConfiguration RestClient::sslConfiguration() const
{
	return d->sslConfig;
}

RequestBuilder RestClient::builder() const
{
	return RequestBuilder(d->nam, d->baseUrl)
			.setVersion(d->apiVersion)
			.addHeaders(d->headers)
			.addParameters(d->query)
			.setSslConfig(d->sslConfig);
}

void RestClient::setBaseUrl(QUrl baseUrl)
{
	if (d->baseUrl == baseUrl)
		return;

	d->baseUrl = baseUrl;
	emit baseUrlChanged(baseUrl, {});
}

void RestClient::setApiVersion(QVersionNumber apiVersion)
{
	if (d->apiVersion == apiVersion)
		return;

	d->apiVersion = apiVersion;
	emit apiVersionChanged(apiVersion, {});
}

void RestClient::setGlobalHeaders(HeaderHash globalHeaders)
{
	if (d->headers == globalHeaders)
		return;

	d->headers = globalHeaders;
	emit globalHeadersChanged(globalHeaders, {});
}

void RestClient::setGlobalParameters(QUrlQuery globalParameters)
{
	if (d->query == globalParameters)
		return;

	d->query = globalParameters;
	emit globalParametersChanged(globalParameters, {});
}

void RestClient::setSslConfiguration(QSslConfiguration sslConfiguration)
{
	if (d->sslConfig == sslConfiguration)
		return;

	d->sslConfig = sslConfiguration;
	emit sslConfigurationChanged(sslConfiguration, {});
}

void RestClient::addGlobalHeader(QByteArray name, QByteArray value)
{
	d->headers.insert(name, value);
	emit globalHeadersChanged(d->headers, {});
}

void RestClient::removeGlobalHeader(QByteArray name)
{
	if(d->headers.remove(name) > 0)
		emit globalHeadersChanged(d->headers, {});
}

void RestClient::addGlobalParameter(QString name, QString value)
{
	d->query.addQueryItem(name, value);
	emit globalParametersChanged(d->query, {});
}

void RestClient::removeGlobalParameter(QString name)
{
	d->query.removeQueryItem(name);
	emit globalParametersChanged(d->query, {});
}

// ------------- Private Implementation -------------

QNetworkAccessManager *RestClientPrivate::getNam(RestClient *client)
{
	return client->d_ptr->nam;
}

RestClientPrivate::RestClientPrivate(RestClient *q_ptr) :
	baseUrl(),
	apiVersion(),
	headers(),
	query(),
	sslConfig(QSslConfiguration::defaultConfiguration()),
	nam(new QNetworkAccessManager(q_ptr)),
	rootClass(new RestClass(q_ptr, {}, q_ptr)),
	serializer(new JsonSerializer(q_ptr)),
	pagingFactory(new StandardPagingFactory())
{}
