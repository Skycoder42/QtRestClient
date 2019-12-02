#include "restreply.h"
#include "restreply_p.h"
#include "restreplyawaitable.h"
#include "restclass.h"
#include "requestbuilder_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
#include <QtCore/QCborStreamReader>

using namespace QtRestClient;

RestReply::RestReply(QNetworkReply *networkReply, QObject *parent) :
	QObject{parent},
	d{new RestReplyPrivate{networkReply, this}}
{
	d->connectReply(networkReply);
}

RestReply *RestReply::onError(const std::function<void (QString, int, ErrorType)> &handler)
{
	return onError(this, handler);
}

RestReply *RestReply::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	connect(this, &RestReply::error, scope, [handler](const QString &errorString, int error, ErrorType type){
		handler(errorString, error, type);
	});
	return this;
}

RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	return onAllErrors(this, handler);
}

RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	this->onFailed(scope, [handler](int code){
		handler(QString{}, code, FailureError);
	});
	this->onError(scope, handler);
	return this;
}

bool RestReply::autoDelete() const
{
	return d->autoDelete;
}

bool RestReply::allowsEmptyReplies() const
{
	return d->allowEmptyReplies;
}

QNetworkReply *RestReply::networkReply() const
{
	return d->networkReply.data();
}

RestReplyAwaitable RestReply::awaitable()
{
	return RestReplyAwaitable{this};
}

void RestReply::abort()
{
	d->networkReply->abort();
}

void RestReply::retry()
{
	d->retryDelay = 0;
}

void RestReply::retryAfter(int mSecs)
{
	d->retryDelay = mSecs;
}

void RestReply::setAutoDelete(bool autoDelete)
{
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	emit autoDeleteChanged(autoDelete, {});
}

void RestReply::setAllowEmptyReplies(bool allowEmptyReplies)
{
	if (d->allowEmptyReplies == allowEmptyReplies)
		return;

	d->allowEmptyReplies = allowEmptyReplies;
	emit allowEmptyRepliesChanged(d->allowEmptyReplies, {});
}

QByteArray RestReply::jsonTypeName(QJsonValue::Type type)
{
	switch (type) {
	case QJsonValue::Null:
		return "null";
	case QJsonValue::Bool:
		return "bool";
	case QJsonValue::Double:
		return "double";
	case QJsonValue::String:
		return "string";
	case QJsonValue::Array:
		return "array";
	case QJsonValue::Object:
		return "object";
	case QJsonValue::Undefined:
		return "undefined";
	default:
		Q_UNREACHABLE();
		return {};
	}
}

// ------------- Private Implementation -------------

const QByteArray RestReplyPrivate::PropertyBuffer("__QtRestClient_RestReplyPrivate_PropertyBuffer");

QNetworkReply *RestReplyPrivate::compatSend(QNetworkAccessManager *nam, const QNetworkRequest &request, const QByteArray &verb, const QByteArray &body)
{
	QNetworkReply *reply = nullptr;
	if(body.isEmpty())
		reply = nam->sendCustomRequest(request, verb);
	else {
		reply = nam->sendCustomRequest(request, verb, body);
		if(reply)
			reply->setProperty(PropertyBuffer, body);
	}
	return reply;
}

RestReplyPrivate::RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr) :
	QObject(q_ptr),
	networkReply(networkReply),
	q(q_ptr)
{}

RestReplyPrivate::~RestReplyPrivate()
{
	if(networkReply)
		networkReply->deleteLater();
}

void RestReplyPrivate::connectReply(QNetworkReply *reply)
{
	connect(reply, &QNetworkReply::finished,
			this, &RestReplyPrivate::replyFinished);

	//forward some signals
	connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
			q, &RestReply::networkError);
#ifndef QT_NO_SSL
	connect(reply, &QNetworkReply::sslErrors,
			this, &RestReplyPrivate::handleSslErrors);
#endif
	connect(reply, &QNetworkReply::downloadProgress,
			q, &RestReply::downloadProgress);
	connect(reply, &QNetworkReply::uploadProgress,
			q, &RestReply::uploadProgress);

	//completed signal
	connect(q, &RestReply::succeeded,
			q, &RestReply::completed);
	connect(q, &RestReply::failed,
			q, &RestReply::completed);
}

void RestReplyPrivate::replyFinished()
{
	retryDelay = -1;
	const auto status = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	const auto contentType = networkReply->header(QNetworkRequest::ContentTypeHeader).toByteArray();
	const auto contentLength = networkReply->header(QNetworkRequest::ContentLengthHeader).toInt();

	DataType data{std::nullopt};
	std::optional<std::pair<int, QString>> parseError = std::nullopt;
	if (contentLength == 0 && (status == 204 || status >= 300 || allowEmptyReplies)) {  // 204 = NO_CONTENT
		// ok
	} else if (contentType == RequestBuilderPrivate::ContentTypeCbor) {
		QCborStreamReader reader{networkReply};
		data = QCborValue::fromCbor(reader);
		if (const auto error = reader.lastError(); error.c != QCborError::NoError)
			parseError = std::make_pair(error.c, error.toString());
	} else if (contentType == RequestBuilderPrivate::ContentTypeJson) {
		const auto readData = networkReply->readAll();
		QJsonParseError error;
		auto jDoc = QJsonDocument::fromJson(readData, &error);
		if (error.error != QJsonParseError::NoError) {
			parseError = std::make_pair(error.error, error.errorString());
			if (error.error == QJsonParseError::IllegalValue) {
				// try to read again as array, to get valid non obj/arr data
				error = QJsonParseError {};  // clear error
				jDoc = QJsonDocument::fromJson("[" + readData + "]", &error);  // read wrapped as array
				if (error.error == QJsonParseError::NoError) {
					parseError.reset();
					data = jDoc.array().first();
				}
			}
		} else if (jDoc.isObject())
			data = QJsonValue{jDoc.object()};
		else if (jDoc.isArray())
			data = QJsonValue{jDoc.array()};
		else if (jDoc.isNull())
			data = QJsonValue{QJsonValue::Null};
		else
			Q_UNREACHABLE();
	} else
		parseError = std::make_pair(-1, QStringLiteral("Unsupported content type: %1").arg(QString::fromUtf8(contentType)));

	//check "http errors", because they can have data, but only if json is valid
	if (!parseError && status >= 300)  // first: status code error + valid data
		emit q->failed(status, data, {});
	else if (networkReply->error() != QNetworkReply::NoError)  // next: check normal network errors
		emit q->error(networkReply->errorString(), networkReply->error(), RestReply::NetworkError, {});
	else if (parseError)  // next: parsing errors
		emit q->error(parseError->second, parseError->first, RestReply::ParseError, {});
	else {  // no errors, completed!
		emit q->succeeded(status, data, {});
		retryDelay = -1;
	}

	if (retryDelay == 0) {
		retryDelay = -1;
		retryReply();
	} else if (retryDelay > 0) {
		QTimer::singleShot(retryDelay, Qt::PreciseTimer, this, &RestReplyPrivate::retryReply);
		retryDelay = -1;
	} else if (autoDelete)
		q->deleteLater();
}

#ifndef QT_NO_SSL
void RestReplyPrivate::handleSslErrors(const QList<QSslError> &errors)
{
	bool ignore = false;
	emit q->sslErrors(errors, ignore);
	if (ignore)
		networkReply->ignoreSslErrors(errors);
}
#endif

void RestReplyPrivate::retryReply()
{
	auto nam = networkReply->manager();
	auto request = networkReply->request();
	auto verb = request.attribute(QNetworkRequest::CustomVerbAttribute, QByteArray{"GET"}).toByteArray();
	auto body = networkReply->property(PropertyBuffer).toByteArray();

	networkReply->deleteLater();
	networkReply = compatSend(nam, request, verb, body);
	connectReply(networkReply);
}
