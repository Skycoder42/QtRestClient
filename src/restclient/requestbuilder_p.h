#ifndef QTRESTCLIENT_REQUESTBUILDER_P_H
#define QTRESTCLIENT_REQUESTBUILDER_P_H

#include "requestbuilder.h"
#include "restclass.h"

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>

namespace QtRestClient {

struct Q_RESTCLIENT_EXPORT RequestBuilderPrivate : public QSharedData
{
	static const QByteArray ContentType;
	static const QByteArray ContentTypeCbor;
	static const QByteArray ContentTypeJson;
	static const QByteArray ContentTypeUrlEncoded;
	static const QByteArray Accept;

	RequestBuilderPrivate(const QUrl &baseUrl, QNetworkAccessManager *nam);
	RequestBuilderPrivate(const RequestBuilderPrivate &other) = default;

	QPointer<QNetworkAccessManager> nam;
	QSharedPointer<RequestBuilder::IExtender> extender;

	QUrl base;
	QVersionNumber version;
	QString user;
	QString pass;
	QStringList path;
	bool trailingSlash = false;
	QUrlQuery query;
	QString fragment;
	HeaderHash headers;
	QHash<QNetworkRequest::Attribute, QVariant> attributes;
#ifndef QT_NO_SSL
	QSslConfiguration sslConfig;
#endif
	QByteArray body;
	QByteArray verb;
	QUrlQuery postQuery;

	void prepareRequest(QNetworkRequest &request, QByteArray *sBody) const;
};

}

#endif // QTRESTCLIENT_REQUESTBUILDER_P_H
