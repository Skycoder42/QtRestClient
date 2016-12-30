#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include "qtrestclient_global.h"
#include "requestbuilder.h"

#include <QNetworkRequest>
#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QVersionNumber>

namespace QtRestClient {

class RestClientPrivate;
class QTRESTCLIENTSHARED_EXPORT RestClient : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
	Q_PROPERTY(QVersionNumber apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
	Q_PROPERTY(HeaderHash globalHeaders READ globalHeaders WRITE setGlobalHeaders NOTIFY globalHeadersChanged)
	Q_PROPERTY(QUrlQuery globalParameters READ globalParameters WRITE setGlobalParameters NOTIFY globalParametersChanged)
	Q_PROPERTY(QSslConfiguration sslConfiguration READ sslConfiguration WRITE setSslConfiguration NOTIFY sslConfigurationChanged)

public:
	explicit RestClient(QObject *parent = nullptr);
	~RestClient();

	QUrl baseUrl() const;
	QVersionNumber apiVersion() const;	
	HeaderHash globalHeaders() const;
	QUrlQuery globalParameters() const;	
	QSslConfiguration sslConfiguration() const;

	RequestBuilder builder() const;

public slots:
	void setBaseUrl(QUrl baseUrl);
	void setApiVersion(QVersionNumber apiVersion);	
	void setGlobalHeaders(HeaderHash globalHeaders);
	void setGlobalParameters(QUrlQuery globalParameters);
	void setSslConfiguration(QSslConfiguration sslConfiguration);

	void addGlobalHeader(QByteArray name, QByteArray value);
	void removeGlobalHeader(QByteArray name);

	void addGlobalParameter(QString name, QString value);
	void removeGlobalParameter(QString name);

signals:
	void baseUrlChanged(QUrl baseUrl);
	void apiVersionChanged(QVersionNumber apiVersion);	
	void globalHeadersChanged(HeaderHash globalHeaders);
	void globalParametersChanged(QUrlQuery globalParameters);	
	void sslConfigurationChanged(QSslConfiguration sslConfiguration);

private:
	QScopedPointer<RestClientPrivate> d_ptr;
};

}

#endif // RESTCLIENT_H
