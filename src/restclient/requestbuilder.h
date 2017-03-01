#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qshareddata.h>

namespace QtRestClient {

struct RequestBuilderPrivate;
class Q_RESTCLIENT_EXPORT RequestBuilder
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
	RequestBuilder &setAttributes(QHash<QNetworkRequest::Attribute, QVariant> attributes);
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
