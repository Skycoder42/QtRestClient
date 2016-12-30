#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include "qtrestclient_global.h"

#include <QObject>
#include <QVersionNumber>

namespace QtRestClient {

class RestClientPrivate;
class QTRESTCLIENTSHARED_EXPORT RestClient : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
	Q_PROPERTY(QVersionNumber apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)

public:
	explicit RestClient(QObject *parent = nullptr);

	QUrl baseUrl() const;
	QVersionNumber apiVersion() const;

public slots:
	void setBaseUrl(QUrl baseUrl);
	void setApiVersion(QVersionNumber apiVersion);

signals:
	void baseUrlChanged(QUrl baseUrl);
	void apiVersionChanged(QVersionNumber apiVersion);

private:
	QUrl m_baseUrl;
	QVersionNumber m_apiVersion;
};

}

#endif // RESTCLIENT_H
