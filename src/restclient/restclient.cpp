#include "restclient.h"
#include "restclient_p.h"
#include "restclass.h"
#include "requestbuilder_p.h"
#include <QtCore/QBitArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
using namespace QtRestClient;

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include "paging_fwd.h"
#include <QtJsonSerializer/JsonSerializer>
#include <QtJsonSerializer/CborSerializer>
using namespace QtJsonSerializer;
#endif

RestClient::RestClient(QObject *parent) :
	  RestClient{DataMode::Json, parent}
{}

RestClient::RestClient(RestClient::DataMode dataMode, QObject *parent) :
	  RestClient{*new RestClientPrivate{}, parent}
{
	setupNam();
	setDataMode(dataMode);
}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
RestClient::RestClient(SerializerBase *serializer, QObject *parent) :
	  RestClient{*new RestClientPrivate{}, parent}
{
	setupNam();
	setSerializer(serializer);
}
#endif

RestClass *RestClient::createClass(const QString &path, QObject *parent)
{
	return new RestClass{this, path.split(QLatin1Char('/'), QString::SkipEmptyParts), parent};
}

RestClass *RestClient::rootClass() const
{
	Q_D(const RestClient);
	return d->rootClass;
}

QNetworkAccessManager *RestClient::manager() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->nam;
}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
SerializerBase *RestClient::serializer() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->serializer;
}
#endif

IPagingFactory *RestClient::pagingFactory() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->pagingFactory.data();
}

RestClient::DataMode RestClient::dataMode() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	return d->serializer && d->serializer->metaObject()->inherits(&CborSerializer::staticMetaObject) ?
		DataMode::Cbor :
		DataMode::Json;
#else
	return d->dataMode;
#endif
}

QUrl RestClient::baseUrl() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->baseUrl;
}

QVersionNumber RestClient::apiVersion() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->apiVersion;
}

HeaderHash RestClient::globalHeaders() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->headers;
}

QUrlQuery RestClient::globalParameters() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->query;
}

QHash<QNetworkRequest::Attribute, QVariant> RestClient::requestAttributes() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->attribs;
}

bool RestClient::isThreaded() const
{
	Q_D(const RestClient);
	return d->threadLock;
}

#ifndef QT_NO_SSL
QSslConfiguration RestClient::sslConfiguration() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->sslConfig;
}
#endif

#ifdef QT_RESTCLIENT_USE_ASYNC
QThreadPool *RestClient::asyncPool() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	return d->asyncPool;
}
#endif

RequestBuilder RestClient::builder() const
{
	Q_D(const RestClient);
	QReadLocker _{d->threadLock};
	RequestBuilder builder{d->baseUrl, d->nam};

	builder.setVersion(d->apiVersion)
		.setAttributes(d->attribs)
#ifndef QT_NO_SSL
		.setSslConfig(d->sslConfig)
#endif
		.addHeaders(d->headers)
		.addParameters(d->query);

	switch (dataMode()) {
	case DataMode::Cbor:
		builder.setAccept(RequestBuilderPrivate::ContentTypeCbor);
		break;
	case DataMode::Json:
		builder.setAccept(RequestBuilderPrivate::ContentTypeJson);
		break;
	default:
		Q_UNREACHABLE();
	}

	return builder;
}

void RestClient::setManager(QNetworkAccessManager *manager)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->nam->deleteLater();
	d->nam = manager;
	manager->setParent(this);
}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
void RestClient::setSerializer(SerializerBase *serializer)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->serializer == serializer)
		return;

	if (d->serializer)
		d->serializer->deleteLater();
	d->serializer = serializer;
	serializer->setParent(this);
	_.unlock();
	Q_EMIT dataModeChanged(dataMode(), {});
}
#endif

void RestClient::setPagingFactory(IPagingFactory *factory)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->pagingFactory.reset(factory);
}

void RestClient::setDataMode(RestClient::DataMode dataMode)
{
	Q_D(RestClient);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	if (this->dataMode() == dataMode && d->serializer)
		return;

	SerializerBase *ser;
	switch (dataMode) {
	case DataMode::Cbor:
		ser = new CborSerializer{this};
		break;
	case DataMode::Json:
		ser = new JsonSerializer{this};
		break;
	default:
		Q_UNREACHABLE();
	}
	ser->setAllowDefaultNull(true);
	setSerializer(ser);
#else
	QWriteLocker _{d->threadLock};
	if (d->dataMode == dataMode)
		return;

	d->dataMode = dataMode;
	Q_EMIT dataModeChanged(d->dataMode, {});
#endif
}

void RestClient::setBaseUrl(QUrl baseUrl)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->baseUrl == baseUrl)
		return;

	d->baseUrl = std::move(baseUrl);
	Q_EMIT baseUrlChanged(d->baseUrl, {});
}

void RestClient::setApiVersion(QVersionNumber apiVersion)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->apiVersion == apiVersion)
		return;

	d->apiVersion = std::move(apiVersion);
	Q_EMIT apiVersionChanged(d->apiVersion, {});
}

void RestClient::setGlobalHeaders(HeaderHash globalHeaders)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->headers == globalHeaders)
		return;

	d->headers = std::move(globalHeaders);
	Q_EMIT globalHeadersChanged(d->headers, {});
}

void RestClient::setGlobalParameters(QUrlQuery globalParameters)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->query == globalParameters)
		return;

	d->query = std::move(globalParameters);
	Q_EMIT globalParametersChanged(d->query, {});
}

void RestClient::setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->attribs == requestAttributes)
		return;

	d->attribs = std::move(requestAttributes);
	Q_EMIT requestAttributesChanged(d->attribs, {});
}

void RestClient::setModernAttributes()
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->attribs.insert(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::SpdyAllowedAttribute, true);
	d->attribs.insert(QNetworkRequest::HTTP2AllowedAttribute, true);
	Q_EMIT requestAttributesChanged(d->attribs, {});
}

void RestClient::setThreaded(bool threaded)
{
	Q_D(RestClient);
	if (static_cast<bool>(d->threadLock) == threaded)
		return;

	if (threaded)
		d->threadLock.storeRelease(new QReadWriteLock{QReadWriteLock::Recursive});
	else {
		auto ptr = d->threadLock.fetchAndStoreOrdered(nullptr);
		delete ptr;
	}
	Q_EMIT threadedChanged(d->threadLock, {});
}

#ifndef QT_NO_SSL
void RestClient::setSslConfiguration(QSslConfiguration sslConfiguration)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->sslConfig == sslConfiguration)
		return;

	d->sslConfig = std::move(sslConfiguration);
	Q_EMIT sslConfigurationChanged(d->sslConfig, {});
}
#endif

#ifdef QT_RESTCLIENT_USE_ASYNC
void RestClient::setAsyncPool(QThreadPool *asyncPool)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if (d->asyncPool == asyncPool)
		return;

	d->asyncPool = asyncPool;
	if (d->asyncPool)
		setThreaded(true);
	Q_EMIT asyncPoolChanged(d->asyncPool, {});
}
#endif

void RestClient::addGlobalHeader(const QByteArray &name, const QByteArray &value)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->headers.insert(name, value);
	Q_EMIT globalHeadersChanged(d->headers, {});
}

void RestClient::removeGlobalHeader(const QByteArray &name)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	if(d->headers.remove(name) > 0)
		Q_EMIT globalHeadersChanged(d->headers, {});
}

void RestClient::addGlobalParameter(const QString &name, const QString &value)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->query.addQueryItem(name, value);
	Q_EMIT globalParametersChanged(d->query, {});
}

void RestClient::removeGlobalParameter(const QString &name)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->query.removeQueryItem(name);
	Q_EMIT globalParametersChanged(d->query, {});
}

void RestClient::addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->attribs.insert(attribute, value);
	Q_EMIT requestAttributesChanged(d->attribs, {});
}

void RestClient::removeRequestAttribute(QNetworkRequest::Attribute attribute)
{
	Q_D(RestClient);
	QWriteLocker _{d->threadLock};
	d->attribs.remove(attribute);
	Q_EMIT requestAttributesChanged(d->attribs, {});
}

RestClient::RestClient(RestClientPrivate &dd, QObject *parent) :
	  QObject{dd, parent}
{
	Q_D(RestClient);
	d->pagingFactory.reset(new StandardPagingFactory{});
	d->rootClass = new RestClass{this, {}, this};
}

void RestClient::setupNam()
{
	Q_D(RestClient);
	Q_ASSERT_X(!d->nam, Q_FUNC_INFO, "RestClient::setupNam can only be called once");
	d->nam = new QNetworkAccessManager{this};
	d->nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

// ------------- Private Implementation -------------

#ifndef DOXYGEN_RUN
QReadWriteLock RestClientPrivate::globalApiLock;
QHash<QString, RestClient*> RestClientPrivate::globalApis;

RestClientPrivate::~RestClientPrivate()
{
	auto ptr = threadLock.fetchAndStoreOrdered(nullptr);
	delete ptr;
}
#endif

// ------------- Global header implementation -------------

Q_LOGGING_CATEGORY(QtRestClient::logGlobal, "qt.restclient");
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
Q_LOGGING_CATEGORY(QtRestClient::logPaging, "qt.restclient.Paging")
#endif

/*!
@param name The name to identify the API with. Use to obtain a reference to the API later on
@param client The RestClient to be registered
@returns `true` if added, `false` if the name is already taken

@attention QtRestClient takes ownership of the `client`, do not delete it after adding it

@sa RestClient, QtRestClient::apiClient, QtRestClient::removeGlobalApi
*/
bool QtRestClient::addGlobalApi(const QString &name, RestClient *client)
{
	QWriteLocker _{&RestClientPrivate::globalApiLock};
	if (RestClientPrivate::globalApis.contains(name))
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
	QWriteLocker _{&RestClientPrivate::globalApiLock};
	if (deleteClient) {
		auto client = RestClientPrivate::globalApis.take(name);
		if (client)
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
	QReadLocker _{&RestClientPrivate::globalApiLock};
	return RestClientPrivate::globalApis.value(name, nullptr);
}

/*!
@param name The name of the root class to be returned
@returns The root `RestClass` for the given API name, or `nullptr` if no such API exists

@sa RestClient::rootClass, QtRestClient::createApiClass, QtRestClient::addGlobalApi
*/
RestClass *QtRestClient::apiRootClass(const QString &name)
{
	QReadLocker _{&RestClientPrivate::globalApiLock};
	auto client = RestClientPrivate::globalApis.value(name, nullptr);
	if (client)
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
	QReadLocker _{&RestClientPrivate::globalApiLock};
	auto client = RestClientPrivate::globalApis.value(name, nullptr);
	if (client)
		return client->createClass(path, parent);
	else
		return nullptr;
}
