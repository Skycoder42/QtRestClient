#include "restclient.h"
#include "restclient_p.h"
#include "restclass.h"
#include "standardpaging.h"
#include <QBitArray>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QUuid>
using namespace QtRestClient;

#define d d_ptr

static void qtRestClientStartup();
Q_COREAPP_STARTUP_FUNCTION(qtRestClientStartup)

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

QNetworkAccessManager *RestClient::manager() const
{
	return d->nam;
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
	serializer(new JsonSerializer(q_ptr)),
	pagingFactory(new StandardPagingFactory()),
	rootClass(new RestClass(q_ptr, {}, q_ptr))
{}

// ------------- Startup function implementation -------------

static void qtRestClientStartup()
{
	registerListConverters<bool>();
	registerListConverters<int>();
	registerListConverters<unsigned int>();
	registerListConverters<double>();
	registerListConverters<QChar>();
	registerListConverters<QString>();
	registerListConverters<long long>();
	registerListConverters<short>();
	registerListConverters<char>();
	registerListConverters<unsigned long>();
	registerListConverters<unsigned long long>();
	registerListConverters<unsigned short>();
	registerListConverters<signed char>();
	registerListConverters<unsigned char>();
	registerListConverters<float>();
	registerListConverters<QDate>();
	registerListConverters<QTime>();
	registerListConverters<QUrl>();
	registerListConverters<QDateTime>();
	registerListConverters<QUuid>();
	registerListConverters<RestObject*>();
}
