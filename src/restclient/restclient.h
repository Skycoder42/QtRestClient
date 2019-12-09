#ifndef QTRESTCLIENT_RESTCLIENT_H
#define QTRESTCLIENT_RESTCLIENT_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/requestbuilder.h"

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qversionnumber.h>
#ifdef QT_RESTCLIENT_USE_ASYNC
#include <QtCore/qthreadpool.h>
#endif

#include <QtNetwork/qnetworkrequest.h>

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
namespace QtJsonSerializer {
class SerializerBase;
}
#endif

namespace QtRestClient {

class RestClass;
class IPagingFactory;

class RestClientPrivate;
//! A class to define access to an API, with general settings
class Q_RESTCLIENT_EXPORT RestClient : public QObject
{
	Q_OBJECT
	friend class RestClientPrivate;

	//! The data mode the client is currently in
	Q_PROPERTY(DataMode dataMode READ dataMode WRITE setDataMode NOTIFY dataModeChanged)
	//! The base URL to be used for every request to that api
	Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
	//! The version number to be appended to the path
	Q_PROPERTY(QVersionNumber apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
	//! A collection of headers to be added to every request
	Q_PROPERTY(HeaderHash globalHeaders READ globalHeaders WRITE setGlobalHeaders NOTIFY globalHeadersChanged)
	//! A URL Query with parameters to be added to every request
	Q_PROPERTY(QUrlQuery globalParameters READ globalParameters WRITE setGlobalParameters NOTIFY globalParametersChanged)
	//! A collection of attributes to be set on every request
	Q_PROPERTY(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes READ requestAttributes WRITE setRequestAttributes NOTIFY requestAttributesChanged)
	//! Specifies, whether the client can be used in a multithreaded context
	Q_PROPERTY(bool threaded READ isThreaded WRITE setThreaded NOTIFY threadedChanged)

#ifndef QT_NO_SSL
	//! The SSL configuration to be used for HTTPS
	Q_PROPERTY(QSslConfiguration sslConfiguration READ sslConfiguration WRITE setSslConfiguration NOTIFY sslConfigurationChanged)
#endif

#ifdef QT_RESTCLIENT_USE_ASYNC
	//! Holds a thread pool to be used by all replies created via this clients classes
	Q_PROPERTY(QThreadPool* asyncPool READ asyncPool WRITE setAsyncPool NOTIFY asyncPoolChanged)
#endif

public:
	//! The different data modes in which the client can operate
	enum class DataMode {
		Cbor,  //!< The client expects and sends data in the binary CBOR format
		Json  //!< The client expects and sends data in the textual JSON format
	};
	Q_ENUM(DataMode)

	//! Constructor
	explicit RestClient(QObject *parent = nullptr);
	//! Constructor with a data mode
	explicit RestClient(DataMode dataMode, QObject *parent = nullptr);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Constructor with a serializer
	explicit RestClient(QtJsonSerializer::SerializerBase *serializer, QObject *parent = nullptr);
#endif

	//! Creates a new rest class for the given path and parent
	RestClass *createClass(const QString &path, QObject *parent = nullptr);
	//! Returns the rest class with the root path
	RestClass *rootClass() const;

	//! Returns the network access manager used by the restclient
	QNetworkAccessManager *manager() const;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Returns the json serializer used by the restclient
	QtJsonSerializer::SerializerBase *serializer() const;
#endif
	//! Returns the paging factory used by the restclient
	IPagingFactory *pagingFactory() const;

	//! @readAcFn{RestClient::dataMode}
	DataMode dataMode() const;
	//! @readAcFn{RestClient::baseUrl}
	QUrl baseUrl() const;
	//! @readAcFn{RestClient::apiVersion}
	QVersionNumber apiVersion() const;
	//! @readAcFn{RestClient::globalHeaders}
	HeaderHash globalHeaders() const;
	//! @readAcFn{RestClient::globalParameters}
	QUrlQuery globalParameters() const;
	//! @readAcFn{RestClient::requestAttributes}
	QHash<QNetworkRequest::Attribute, QVariant> requestAttributes() const;
	//! @readAcFn{RestClient::threaded}
	bool isThreaded() const;
#ifndef QT_NO_SSL
	//! @readAcFn{RestClient::sslConfiguration}
	QSslConfiguration sslConfiguration() const;
#endif
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @readAcFn{RestClient::asyncPool}
	QThreadPool* asyncPool() const;
#endif

	//! Creates a request builder with all the settings of this client
	virtual RequestBuilder builder() const;

public Q_SLOTS:
	//! Sets the network access manager to be used by all requests for this client
	void setManager(QNetworkAccessManager *manager);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Sets the json serializer to be used by all requests for this client
	void setSerializer(QtJsonSerializer::SerializerBase *serializer);
#endif

	//! Sets the paging factory to be used by all paging requests for this client
	void setPagingFactory(IPagingFactory *factory);

	//! @writeAcFn{RestClient::dataMode}
	void setDataMode(DataMode dataMode);
	//! @writeAcFn{RestClient::baseUrl}
	void setBaseUrl(QUrl baseUrl);
	//! @writeAcFn{RestClient::apiVersion}
	void setApiVersion(QVersionNumber apiVersion);
	//! @writeAcFn{RestClient::globalHeaders}
	void setGlobalHeaders(HeaderHash globalHeaders);
	//! @writeAcFn{RestClient::globalParameters}
	void setGlobalParameters(QUrlQuery globalParameters);
	//! @writeAcFn{RestClient::requestAttributes}
	void setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes);
	//! Sets modern attributes in RestClient::requestAttributes
	void setModernAttributes();
	//! @writeAcFn{RestClient::threaded}
	void setThreaded(bool threaded);
#ifndef QT_NO_SSL
	//! @writeAcFn{RestClient::sslConfiguration}
	void setSslConfiguration(QSslConfiguration sslConfiguration);
#endif
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @writeAcFn{RestClient::asyncPool}
	void setAsyncPool(QThreadPool* asyncPool);
#endif

	//! @writeAcFn{RestClient::globalHeaders}
	void addGlobalHeader(const QByteArray &name, const QByteArray &value);
	//! @writeAcFn{RestClient::globalHeaders}
	void removeGlobalHeader(const QByteArray &name);

	//! @writeAcFn{RestClient::globalParameters}
	void addGlobalParameter(const QString &name, const QString &value);
	//! @writeAcFn{RestClient::globalParameters}
	void removeGlobalParameter(const QString &name);

	//! @writeAcFn{RestClient::requestAttributes}
	void addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
	//! @writeAcFn{RestClient::requestAttributes}
	void removeRequestAttribute(QNetworkRequest::Attribute attribute);

Q_SIGNALS:
	//! @notifyAcFn{RestClient::dataMode}
	void dataModeChanged(DataMode dataMode, QPrivateSignal);
	//! @notifyAcFn{RestClient::baseUrl}
	void baseUrlChanged(QUrl baseUrl, QPrivateSignal);
	//! @notifyAcFn{RestClient::apiVersion}
	void apiVersionChanged(QVersionNumber apiVersion, QPrivateSignal);
	//! @notifyAcFn{RestClient::globalHeaders}
	void globalHeadersChanged(HeaderHash globalHeaders, QPrivateSignal);
	//! @notifyAcFn{RestClient::globalParameters}
	void globalParametersChanged(QUrlQuery globalParameters, QPrivateSignal);
	//! @notifyAcFn{RestClient::requestAttributes}
	void requestAttributesChanged(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes, QPrivateSignal);
	//! @notifyAcFn{RestClient::threaded}
	void threadedChanged(bool threaded, QPrivateSignal);
#ifndef QT_NO_SSL
	//! @notifyAcFn{RestClient::sslConfiguration}
	void sslConfigurationChanged(QSslConfiguration sslConfiguration, QPrivateSignal);
#endif
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @notifyAcFn{RestClient::asyncPool}
	void asyncPoolChanged(QThreadPool* asyncPool, QPrivateSignal);
#endif

protected:
	//! @private
	RestClient(RestClientPrivate &dd, QObject *parent = nullptr);
	//! @private
	void setupNam();

private:
	Q_DECLARE_PRIVATE(RestClient)
};

}

#endif // QTRESTCLIENT_RESTCLIENT_H
