#include "requestbuilder.h"

#include <QBuffer>
#include <QJsonDocument>
using namespace QtRestClient;

namespace QtRestClient {
struct RequestBuilderPrivate
{
	static QByteArray ContentType;
	static QByteArray ContentTypeJson;

	QNetworkAccessManager *nam;

	QUrl base;
	QVersionNumber version;
	QStringList path;
	QUrlQuery query;
	HeaderHash headers;
	QHash<QNetworkRequest::Attribute, QVariant> attributes;
	QByteArray body;
	QByteArray verb;

	inline RequestBuilderPrivate(QNetworkAccessManager *nam = nullptr, QUrl baseUrl = QUrl()) :
		nam(nam),
		base(baseUrl),
		version(),
		path(),
		query(),
		headers(),
		attributes({{QNetworkRequest::FollowRedirectsAttribute, true}}),
		body(),
		verb("GET")
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
	d_ptr(new RequestBuilderPrivate())
{
	*d = *(other.d);
}

RequestBuilder::~RequestBuilder()
{
	delete d_ptr;
}

RequestBuilder &RequestBuilder::setVersion(const QVersionNumber &version)
{
	d->version = version;
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

RequestBuilder &RequestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
	d->attributes.insert(attribute, value);
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

QNetworkRequest RequestBuilder::build()
{
	auto url = d->base;

	auto pathList = d->path;
	if(!d->version.isNull())
		pathList.prepend(d->version.toString());
	auto path = url.path();
	if(!pathList.isEmpty() && !path.endsWith(QLatin1Char('/'))){
		path.append(QLatin1Char('/'));
		path.append(pathList.join(QLatin1Char('/')));
	}
	url.setPath(path);

	url.setQuery(d->query);

	QNetworkRequest request(url);
	for(auto it = d->headers.constBegin(); it != d->headers.constEnd(); it++)
		request.setRawHeader(it.key(), it.value());
	for(auto it = d->attributes.constBegin(); it != d->attributes.constEnd(); it++)
		request.setAttribute(it.key(), it.value());
	return request;
}

QNetworkReply *RequestBuilder::send()
{
	auto request = build();

	QBuffer *buffer = nullptr;
	if(!d->body.isEmpty()) {
		buffer = new QBuffer();
		buffer->setData(d->body);
		buffer->open(QIODevice::ReadOnly);
	}

	auto reply = d->nam->sendCustomRequest(request, d->verb, buffer);

	if(buffer) {
		if(reply) {
			QObject::connect(reply, &QNetworkReply::destroyed, d->nam, [=](){
				buffer->close();
				buffer->deleteLater();
			});
		} else {
			buffer->close();
			buffer->deleteLater();
		}
	}

	return reply;
}
