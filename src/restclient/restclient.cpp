#include "restclient.h"
#include "restclient_p.h"
#include "restclass.h"
#include "standardpaging.h"
#include <QtCore/QBitArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
using namespace QtRestClient;

#define d d_ptr

RestClient::RestClient(QObject *parent) :
	QObject(parent),
	d_ptr(new RestClientPrivate(this))
{}

RestClient::~RestClient() {}

RestClass *RestClient::createClass(const QString &path, QObject *parent)
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

QJsonSerializer *RestClient::serializer() const
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

QHash<QNetworkRequest::Attribute, QVariant> RestClient::requestAttributes() const
{
	return d->attribs;
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
			.setAttributes(d->attribs)
			.setSslConfig(d->sslConfig);
}

void RestClient::setManager(QNetworkAccessManager *manager)
{
	d->nam->deleteLater();
	d->nam = manager;
	manager->setParent(this);
}

void RestClient::setSerializer(QJsonSerializer *serializer)
{
	d->serializer->deleteLater();
	d->serializer = serializer;
	serializer->setParent(this);
}

void RestClient::setPagingFactory(PagingFactory *factory)
{
	d->pagingFactory.reset(factory);
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

void RestClient::setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes)
{
	if (d->attribs == requestAttributes)
		return;

	d->attribs = requestAttributes;
	emit requestAttributesChanged(requestAttributes, {});
}

void RestClient::setModernAttributes()
{
	d->attribs.insert(QNetworkRequest::FollowRedirectsAttribute, true);
	d->attribs.insert(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::SpdyAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::HTTP2AllowedAttribute, true);
	emit requestAttributesChanged(d->attribs, {});
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

void RestClient::addRequestAttribute(QNetworkRequest::Attribute attribute, QVariant value)
{
	d->attribs.insert(attribute, value);
	emit requestAttributesChanged(d->attribs, {});
}

void RestClient::removeRequestAttribute(QNetworkRequest::Attribute attribute)
{
	d->attribs.remove(attribute);
	emit requestAttributesChanged(d->attribs, {});
}

// ------------- Private Implementation -------------

QHash<QString, RestClient*> RestClientPrivate::globalApis;

RestClientPrivate::RestClientPrivate(RestClient *q_ptr) :
	baseUrl(),
	apiVersion(),
	headers(),
	query(),
	attribs(),
	sslConfig(QSslConfiguration::defaultConfiguration()),
	nam(new QNetworkAccessManager(q_ptr)),
	serializer(new QJsonSerializer(q_ptr)),
	pagingFactory(new StandardPagingFactory()),
	rootClass(new RestClass(q_ptr, {}, q_ptr))
{}

// ------------- Global header implementation -------------

/*!
@param name The name to identify the API with. Use to obtain a reference to the API later on
@param client The RestClient to be registered
@returns `true` if added, `false` if the name is already taken

@Attention QtRestClient takes ownership of the `client`, do not delete it after adding it

@sa RestClient, QtRestClient::apiClient, QtRestClient::removeGlobalApi
*/
bool QtRestClient::addGlobalApi(const QString &name, RestClient *client)
{
	if(RestClientPrivate::globalApis.contains(name))
		return false;
	else {
		client->setParent(qApp);
		RestClientPrivate::globalApis.insert(name, client);
		return true;
	}
}

/*!
@param name The name of the API to be removed.
@param deleteClient Specifies whether the removed client should be deleted

If you don't delete the client, It will be automatically deleted together with the QCoreApplication. If you don't want this, simply retake ownership by yourself:

@code{.cpp}
auto client = QtRestClient::apiClient("name");
QtRestClient::removeGlobalApi("name");
client->setParent(this);
@endcode

@sa QtRestClient::addGlobalApi
*/
void QtRestClient::removeGlobalApi(const QString &name, bool deleteClient)
{
	if(deleteClient) {
		auto client = RestClientPrivate::globalApis.take(name);
		if(client)
			client->deleteLater();
	} else
		RestClientPrivate::globalApis.remove(name);
}

/*!
@param name The name of the root class to be returned
@returns The `RestClient` for the given API name, or `nullptr` if no such API exists

@sa QtRestClient::addGlobalApi
*/
RestClient *QtRestClient::apiClient(const QString &name)
{
	return RestClientPrivate::globalApis.value(name, nullptr);
}

/*!
@param name The name of the root class to be returned
@returns The root `RestClass` for the given API name, or `nullptr` if no such API exists

@sa RestClient::rootClass, QtRestClient::createApiClass, QtRestClient::addGlobalApi
*/
RestClass *QtRestClient::apiRootClass(const QString &name)
{
	auto client = RestClientPrivate::globalApis.value(name, nullptr);
	if(client)
		return client->rootClass();
	else
		return nullptr;
}

/*!
@param name The name of the root class to be returned
@param path The path to be used for the `RestClass`
@param parent The parent object for the created class
@returns A newly created `RestClass` for the given API name, or `nullptr` if no such API exists

@sa RestClient::createClass, QtRestClient::apiRootClass, QtRestClient::addGlobalApi
*/
RestClass *QtRestClient::createApiClass(const QString &name, const QString &path, QObject *parent)
{
	auto client = RestClientPrivate::globalApis.value(name, nullptr);
	if(client)
		return client->createClass(path, parent);
	else
		return nullptr;
}
