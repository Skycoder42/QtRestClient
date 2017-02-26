#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

#include "qtrestclient_global.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QVersionNumber>
#include <QSharedDataPointer>

namespace QtRestClient {

struct RequestBuilderPrivate;
class QTRESTCLIENTSHARED_EXPORT RequestBuilder
{
public:
	RequestBuilder(QNetworkAccessManager *nam, QUrl baseUrl);
	RequestBuilder(const RequestBuilder &other);
	~RequestBuilder();

	RequestBuilder &setCredentials(const QString &user, const QString &password);
	RequestBuilder &setVersion(const QVersionNumber &version);
	RequestBuilder &addPath(QString pathSegment);
	RequestBuilder &addPath(QStringList pathSegment);
	RequestBuilder &trailingSlash();
	RequestBuilder &addParameter(const QString &name, const QString &value);
	RequestBuilder &addParameters(const QUrlQuery &parameters);
	RequestBuilder &setFragment(const QString &fragment);
	RequestBuilder &addHeader(const QByteArray &name, const QByteArray &value);
	RequestBuilder &addHeaders(const HeaderHash &headers);

	RequestBuilder &updateFromRelativeUrl(const QUrl &url, bool mergeQuery = false, bool keepFragment = false);

	RequestBuilder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
	RequestBuilder &setSslConfig(QSslConfiguration sslConfig);

	RequestBuilder &setBody(const QByteArray &body, const QByteArray &contentType);
	RequestBuilder &setBody(const QJsonObject &body);
	RequestBuilder &setBody(const QJsonArray &body);
	RequestBuilder &setVerb(const QByteArray &verb);

	QUrl buildUrl() const;
	QNetworkRequest build() const;
	QNetworkReply *send() const;

	RequestBuilder &operator =(const RequestBuilder &other);

private:
	QSharedDataPointer<RequestBuilderPrivate> d_ptr;
};

}

#endif // REQUESTBUILDER_H
