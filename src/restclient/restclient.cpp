#include "restclient.h"
#include "restclient_p.h"
#include "restclass.h"
#include "standardpaging_p.h"
#include <QtCore/QBitArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
using namespace QtRestClient;

RestClient::RestClient(QObject *parent) :
	QObject(parent),
	d(new RestClientPrivate(this))
{
	d->serializer->setAllowDefaultNull(true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
	d->nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
}

RestClient::~RestClient() = default;

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

#ifndef QT_NO_SSL
QSslConfiguration RestClient::sslConfiguration() const
{
	return d->sslConfig;
}
#endif

RequestBuilder RestClient::builder() const
{
	return RequestBuilder(d->baseUrl, d->nam)
#ifndef QT_NO_SSL
			.setSslConfig(d->sslConfig)
#endif
			.setVersion(d->apiVersion)
			.addHeaders(d->headers)
			.addParameters(d->query)
			.setAttributes(d->attribs);
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

	d->baseUrl = std::move(baseUrl);
	emit baseUrlChanged(d->baseUrl, {});
}

void RestClient::setApiVersion(QVersionNumber apiVersion)
{
	if (d->apiVersion == apiVersion)
		return;

	d->apiVersion = std::move(apiVersion);
	emit apiVersionChanged(d->apiVersion, {});
}

void RestClient::setGlobalHeaders(HeaderHash globalHeaders)
{
	if (d->headers == globalHeaders)
		return;

	d->headers = std::move(globalHeaders);
	emit globalHeadersChanged(d->headers, {});
}

void RestClient::setGlobalParameters(QUrlQuery globalParameters)
{
	if (d->query == globalParameters)
		return;

	d->query = std::move(globalParameters);
	emit globalParametersChanged(d->query, {});
}

void RestClient::setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes)
{
	if (d->attribs == requestAttributes)
		return;

	d->attribs = std::move(requestAttributes);
	emit requestAttributesChanged(d->attribs, {});
}

void RestClient::setModernAttributes()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
	d->attribs.insert(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
	d->attribs.insert(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::SpdyAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::HTTP2AllowedAttribute, true);
	emit requestAttributesChanged(d->attribs, {});
}

#ifndef QT_NO_SSL
void RestClient::setSslConfiguration(QSslConfiguration sslConfiguration)
{
	if (d->sslConfig == sslConfiguration)
		return;

	d->sslConfig = std::move(sslConfiguration);
	emit sslConfigurationChanged(d->sslConfig, {});
}
#endif

void RestClient::addGlobalHeader(const QByteArray &name, const QByteArray &value)
{
	d->headers.insert(name, value);
	emit globalHeadersChanged(d->headers, {});
}

void RestClient::removeGlobalHeader(const QByteArray &name)
{
	if(d->headers.remove(name) > 0)
		emit globalHeadersChanged(d->headers, {});
}

void RestClient::addGlobalParameter(const QString &name, const QString &value)
{
	d->query.addQueryItem(name, value);
	emit globalParametersChanged(d->query, {});
}

void RestClient::removeGlobalParameter(const QString &name)
{
	d->query.removeQueryItem(name);
	emit globalParametersChanged(d->query, {});
}

void RestClient::addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
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
#ifndef QT_NO_SSL
	sslConfig{QSslConfiguration::defaultConfiguration()},
#endif
	nam{new QNetworkAccessManager{q_ptr}},
	serializer{new QJsonSerializer{q_ptr}},
	pagingFactory{new StandardPagingFactory{}},
	rootClass{new RestClass{q_ptr, {}, q_ptr}}
{}

// ------------- Global header implementation -------------

/*!
@param name The name to identify the API with. Use to obtain a reference to the API later on
@param client The RestClient to be registered
@returns `true` if added, `false` if the name is already taken

@attention QtRestClient takes ownership of the `client`, do not delete it after adding it

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
