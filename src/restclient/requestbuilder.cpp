#include "requestbuilder.h"
#include "restreply_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
using namespace QtRestClient;

namespace QtRestClient {
struct RequestBuilderPrivate : public QSharedData
{
	static QByteArray ContentType;
	static QByteArray ContentTypeJson;

	QNetworkAccessManager *nam;

	QUrl base;
	QVersionNumber version;
	QString user;
	QString pass;
	QStringList path;
	bool trailingSlash;
	QUrlQuery query;
	QString fragment;
	HeaderHash headers;
	QHash<QNetworkRequest::Attribute, QVariant> attributes;
	QSslConfiguration sslConfig;
	QByteArray body;
	QByteArray verb;

	inline RequestBuilderPrivate(QNetworkAccessManager *nam = nullptr, QUrl baseUrl = QUrl()) :
		QSharedData(),
		nam(nam),
		base(baseUrl),
		version(),
		user(baseUrl.userName()),
		pass(baseUrl.password()),
		path(),
		trailingSlash(false),
		query(baseUrl.query()),
		fragment(baseUrl.fragment()),
		headers(),
		attributes({{QNetworkRequest::FollowRedirectsAttribute, true}}),
		sslConfig(QSslConfiguration::defaultConfiguration()),
		body(),
		verb("GET")
	{}

	inline RequestBuilderPrivate(const RequestBuilderPrivate &other) :
		QSharedData(other),
		nam(other.nam),
		base(other.base),
		version(other.version),
		user(other.user),
		pass(other.pass),
		path(other.path),
		trailingSlash(other.trailingSlash),
		query(other.query),
		fragment(other.fragment),
		headers(other.headers),
		attributes(other.attributes),
		sslConfig(other.sslConfig),
		body(other.body),
		verb(other.verb)
	{}
};

QByteArray RequestBuilderPrivate::ContentType = "Content-Type";
QByteArray RequestBuilderPrivate::ContentTypeJson = "application/json";
}

#define d d_ptr

RequestBuilder::RequestBuilder(QNetworkAccessManager *nam, QUrl baseUrl) :
	d_ptr(new RequestBuilderPrivate(nam, baseUrl))
{}

RequestBuilder::RequestBuilder(const RequestBuilder &other) :
	d_ptr(other.d_ptr)
{}

RequestBuilder::~RequestBuilder() {}

RequestBuilder &RequestBuilder::setVersion(const QVersionNumber &version)
{
	d->version = version;
	return *this;
}

RequestBuilder &RequestBuilder::setCredentials(const QString &user, const QString &password)
{
	d->user = user;
	d->pass = password;
	return *this;
}

RequestBuilder &RequestBuilder::addHeader(const QByteArray &name, const QByteArray &value)
{
	d->headers.insert(name, value);
	return *this;
}

RequestBuilder &RequestBuilder::addHeaders(const HeaderHash &headers)
{
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		d->headers.insert(it.key(), it.value());
	return *this;
}

RequestBuilder &RequestBuilder::updateFromRelativeUrl(const QUrl &url, bool mergeQuery, bool keepFragment)
{
	auto cUrl = buildUrl();
	d->base = cUrl.resolved(url);
	if(d->base.host() != cUrl.host()) {
		qWarning() << "URL host changed from"
				   << cUrl.host()
				   << "to"
				   << d->base.host();
	}
	//clear all the rest
	d->version = QVersionNumber();
	d->user.clear();
	d->pass.clear();
	d->path.clear();
	if(mergeQuery) {
		QUrlQuery query(url.query());
		foreach(auto item, query.queryItems())
			d->query.addQueryItem(item.first, item.second);
	} else
		d->query = QUrlQuery(url.query());
	if(!keepFragment)
		d->fragment.clear();
	return *this;
}

RequestBuilder &RequestBuilder::addParameter(const QString &name, const QString &value)
{
	d->query.addQueryItem(name, value);
	return *this;
}

RequestBuilder &RequestBuilder::addParameters(const QUrlQuery &parameters)
{
	foreach(auto param, parameters.queryItems())
		d->query.addQueryItem(param.first, param.second);
	return *this;
}

RequestBuilder &RequestBuilder::setFragment(const QString &fragment)
{
	d->fragment = fragment;
	return *this;
}

RequestBuilder &RequestBuilder::addPath(QString pathSegment)
{
	d->path.append(pathSegment.split(QLatin1Char('/'), QString::SkipEmptyParts));
	return *this;
}

RequestBuilder &RequestBuilder::addPath(QStringList pathSegment)
{
	d->path.append(pathSegment);
	return *this;
}

RequestBuilder &RequestBuilder::trailingSlash()
{
	d->trailingSlash = true;
	return *this;
}

RequestBuilder &RequestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
	d->attributes.insert(attribute, value);
	return *this;
}

RequestBuilder &RequestBuilder::setSslConfig(QSslConfiguration sslConfig)
{
	d->sslConfig = sslConfig;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const QByteArray &body, const QByteArray &contentType)
{
	d->body = body;
	d->headers.insert(RequestBuilderPrivate::ContentType, contentType);
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const QJsonObject &body)
{
	d->body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeJson);
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const QJsonArray &body)
{
	d->body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeJson);
	return *this;
}

RequestBuilder &RequestBuilder::setVerb(const QByteArray &verb)
{
	d->verb = verb;
	return *this;
}

QUrl RequestBuilder::buildUrl() const
{
	auto url = d->base;

	auto pathList = url.path().split(QLatin1Char('/'), QString::SkipEmptyParts);
	if(!d->version.isNull())
		pathList.append(QLatin1Char('v') + d->version.normalized().toString());
	pathList.append(d->path);
	url.setPath(QLatin1Char('/') + pathList.join(QLatin1Char('/')) + (d->trailingSlash ? QLatin1String("/") : QString()));

	if(!d->user.isNull())
		url.setUserName(d->user);
	if(!d->pass.isNull())
		url.setPassword(d->pass);
	if(!d->query.isEmpty())
		url.setQuery(d->query);
	if(!d->fragment.isNull())
		url.setFragment(d->fragment);

	return url;
}

QNetworkRequest RequestBuilder::build() const
{
	QNetworkRequest request(buildUrl());
	for(auto it = d->headers.constBegin(); it != d->headers.constEnd(); it++)
		request.setRawHeader(it.key(), it.value());
	for(auto it = d->attributes.constBegin(); it != d->attributes.constEnd(); it++)
		request.setAttribute(it.key(), it.value());
	request.setSslConfiguration(d->sslConfig);
	return request;
}

QNetworkReply *RequestBuilder::send() const
{
	auto request = build();

	QBuffer *buffer = nullptr;
	if(!d->body.isEmpty()) {
		buffer = new QBuffer();
		buffer->setData(d->body);
		buffer->open(QIODevice::ReadOnly);
	}

	return RestReplyPrivate::compatSend(d->nam, request, d->verb, buffer);
}

RequestBuilder &RequestBuilder::operator =(const RequestBuilder &other)
{
	d_ptr = other.d_ptr;
	return *this;
}
