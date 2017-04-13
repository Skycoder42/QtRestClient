#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/requestbuilder.h"

#include <QtNetwork/qnetworkrequest.h>
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qversionnumber.h>
class QJsonSerializer;

namespace QtRestClient {

class RestClass;
class PagingFactory;

class RestClientPrivate;
//! A class to define access to an API, with general settings
class Q_RESTCLIENT_EXPORT RestClient : public QObject
{
	Q_OBJECT
	friend class RestClientPrivate;

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
	//! The SSL configuration to be used for HTTPS
	Q_PROPERTY(QSslConfiguration sslConfiguration READ sslConfiguration WRITE setSslConfiguration NOTIFY sslConfigurationChanged)

public:
	//! Constructor
	explicit RestClient(QObject *parent = nullptr);
	//! Destructor
	~RestClient();

	//! Creates a new rest class for the given path and parent
	RestClass *createClass(const QString &path, QObject *parent = nullptr);
	//! Returns the rest class with the root path
	RestClass *rootClass() const;

	//! Returns the network access manager used by the restclient
	QNetworkAccessManager *manager() const;
	//! Returns the json serializer used by the restclient
	QJsonSerializer *serializer() const;
	//! Returns the paging factory used by the restclient
	PagingFactory *pagingFactory() const;

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
	//! @readAcFn{RestClient::sslConfiguration}
	QSslConfiguration sslConfiguration() const;

	//! Creates a request builder with all the settings of this client
	virtual RequestBuilder builder() const;

public Q_SLOTS:
	//! Sets the network access manager to be used by all requests for this client
	void setManager(QNetworkAccessManager *manager);
	//! Sets the json serializer to be used by all requests for this client
	void setSerializer(QJsonSerializer *serializer);
	//! Sets the paging factory to be used by all paging requests for this client
	void setPagingFactory(PagingFactory *factory);

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
	//! @writeAcFn{RestClient::sslConfiguration}
	void setSslConfiguration(QSslConfiguration sslConfiguration);

	//! @writeAcFn{RestClient::globalHeaders}
	void addGlobalHeader(QByteArray name, QByteArray value);
	//! @writeAcFn{RestClient::globalHeaders}
	void removeGlobalHeader(QByteArray name);

	//! @writeAcFn{RestClient::globalParameters}
	void addGlobalParameter(QString name, QString value);
	//! @writeAcFn{RestClient::globalParameters}
	void removeGlobalParameter(QString name);

	//! @writeAcFn{RestClient::requestAttributes}
	void addRequestAttribute(QNetworkRequest::Attribute attribute, QVariant value);
	//! @writeAcFn{RestClient::requestAttributes}
	void removeRequestAttribute(QNetworkRequest::Attribute attribute);

Q_SIGNALS:
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
	//! @notifyAcFn{RestClient::sslConfiguration}
	void sslConfigurationChanged(QSslConfiguration sslConfiguration, QPrivateSignal);

private:
	QScopedPointer<RestClientPrivate> d;
};

}

#endif // RESTCLIENT_H
