#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include "QtRestClient/qrestclient_global.h"
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
class Q_RESTCLIENT_EXPORT RestClient : public QObject
{
	Q_OBJECT
	friend class RestClientPrivate;

	Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
	Q_PROPERTY(QVersionNumber apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
	Q_PROPERTY(HeaderHash globalHeaders READ globalHeaders WRITE setGlobalHeaders NOTIFY globalHeadersChanged)
	Q_PROPERTY(QUrlQuery globalParameters READ globalParameters WRITE setGlobalParameters NOTIFY globalParametersChanged)
	Q_PROPERTY(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes READ requestAttributes WRITE setRequestAttributes NOTIFY requestAttributesChanged)
	Q_PROPERTY(QSslConfiguration sslConfiguration READ sslConfiguration WRITE setSslConfiguration NOTIFY sslConfigurationChanged)

public:
	explicit RestClient(QObject *parent = nullptr);
	~RestClient();

	RestClass *createClass(const QString &path, QObject *parent = nullptr);
	RestClass *rootClass() const;

	QNetworkAccessManager *manager() const;
	QJsonSerializer *serializer() const;
	PagingFactory *pagingFactory() const;

	QUrl baseUrl() const;
	QVersionNumber apiVersion() const;
	HeaderHash globalHeaders() const;
	QUrlQuery globalParameters() const;
	QHash<QNetworkRequest::Attribute, QVariant> requestAttributes() const;
	QSslConfiguration sslConfiguration() const;

	RequestBuilder builder() const;

	//global access
	static bool addGlobalApi(const QString &name, RestClient *client);
	static void removeGlobalApi(const QString &name, bool deleteClient = true);
	static RestClient *apiClient(const QString &name);
	static RestClass *apiRootClass(const QString &name);
	static RestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);

public Q_SLOTS:
	void setBaseUrl(QUrl baseUrl);
	void setApiVersion(QVersionNumber apiVersion);
	void setGlobalHeaders(HeaderHash globalHeaders);
	void setGlobalParameters(QUrlQuery globalParameters);
	void setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes);
	void setModernAttributes();
	void setSslConfiguration(QSslConfiguration sslConfiguration);

	void addGlobalHeader(QByteArray name, QByteArray value);
	void removeGlobalHeader(QByteArray name);

	void addGlobalParameter(QString name, QString value);
	void removeGlobalParameter(QString name);

	void addRequestAttribute(QNetworkRequest::Attribute attribute, QVariant value);
	void removeRequestAttribute(QNetworkRequest::Attribute attribute);

Q_SIGNALS:
	void baseUrlChanged(QUrl baseUrl, QPrivateSignal);
	void apiVersionChanged(QVersionNumber apiVersion, QPrivateSignal);
	void globalHeadersChanged(HeaderHash globalHeaders, QPrivateSignal);
	void globalParametersChanged(QUrlQuery globalParameters, QPrivateSignal);
	void requestAttributesChanged(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes, QPrivateSignal);
	void sslConfigurationChanged(QSslConfiguration sslConfiguration, QPrivateSignal);

private:
	QScopedPointer<RestClientPrivate> d_ptr;
};

}

#endif // RESTCLIENT_H
