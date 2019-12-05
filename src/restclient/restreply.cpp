#include "restreply.h"
#include "restreply_p.h"
#include "restclass.h"
#include "restreplyawaitable.h"
#include "requestbuilder_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
#include <QtCore/QCborStreamReader>

using namespace QtRestClient;

RestReply::RestReply(QNetworkReply *networkReply, QObject *parent) :
	  RestReply{*new RestReplyPrivate{}, parent}
{
	Q_D(RestReply);
	d->networkReply = networkReply;
	d->connectReply();
}

RestReply::~RestReply()
{
	Q_D(RestReply);
	if (d->networkReply)
		d->networkReply->deleteLater();
}

RestReply *RestReply::onError(std::function<void(QString, int, Error)> handler)
{
	return onError(this, std::move(handler));
}

RestReply *RestReply::onError(QObject *scope, std::function<void (QString, int, RestReply::Error)> handler)
{
	connect(this, &RestReply::error, scope, [xHandler = std::move(handler)](const QString &errorString, int error, Error type){
		xHandler(errorString, error, type);
	});
	return this;
}

RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::Error)> &handler)
{
	return onAllErrors(this, handler);
}

RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::Error)> &handler)
{
	this->onFailed(scope, [handler](int code){
		handler(QString{}, code, Error::Failure);
	});
	this->onError(scope, handler);
	return this;
}

bool RestReply::autoDelete() const
{
	Q_D(const RestReply);
	return d->autoDelete;
}

bool RestReply::allowsEmptyReplies() const
{
	Q_D(const RestReply);
	return d->allowEmptyReplies;
}

QNetworkReply *RestReply::networkReply() const
{
	Q_D(const RestReply);
	return d->networkReply.data();
}

RestReplyAwaitable RestReply::awaitable()
{
	return RestReplyAwaitable{this};
}

void RestReply::abort()
{
	Q_D(RestReply);
	d->networkReply->abort();
}

void RestReply::retry()
{
	Q_D(RestReply);
	d->retryDelay = 0;
}

void RestReply::retryAfter(int mSecs)
{
	Q_D(RestReply);
	d->retryDelay = mSecs;
}

void RestReply::setAutoDelete(bool autoDelete)
{
	Q_D(RestReply);
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	emit autoDeleteChanged(autoDelete, {});
}

void RestReply::setAllowEmptyReplies(bool allowEmptyReplies)
{
	Q_D(RestReply);
	if (d->allowEmptyReplies == allowEmptyReplies)
		return;

	d->allowEmptyReplies = allowEmptyReplies;
	emit allowEmptyRepliesChanged(d->allowEmptyReplies, {});
}

RestReply::RestReply(RestReplyPrivate &dd, QObject *parent) :
	  QObject{dd, parent}
{}

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
	if (body.isEmpty())
		reply = nam->sendCustomRequest(request, verb);
	else {
		reply = nam->sendCustomRequest(request, verb, body);
		if (reply)
			reply->setProperty(PropertyBuffer, body);
	}
	return reply;
}

void RestReplyPrivate::connectReply()
{
	Q_Q(RestReply);
	connect(networkReply, &QNetworkReply::finished,
			this, &RestReplyPrivate::_q_replyFinished);

	//forward some signals
	QObject::connect(networkReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
					 q, &RestReply::networkError);
#ifndef QT_NO_SSL
	connect(networkReply, &QNetworkReply::sslErrors,
			this, &RestReplyPrivate::_q_handleSslErrors);
#endif
	QObject::connect(networkReply, &QNetworkReply::downloadProgress,
					 q, &RestReply::downloadProgress);
	QObject::connect(networkReply, &QNetworkReply::uploadProgress,
					 q, &RestReply::uploadProgress);

	//completed signal
	QObject::connect(q, &RestReply::succeeded,
					 q, &RestReply::completed);
	QObject::connect(q, &RestReply::failed,
					 q, &RestReply::completed);
}

void RestReplyPrivate::_q_replyFinished()
{
	Q_Q(RestReply);
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
	if (!parseError && status >= 300 && !std::holds_alternative<std::nullopt_t>(data))  // first: status code error + valid data
		emit q->failed(status, data, {});
	else if (networkReply->error() != QNetworkReply::NoError)  // next: check normal network errors
		emit q->error(networkReply->errorString(), networkReply->error(), Error::Network, {});
	else if (parseError)  // next: parsing errors
		emit q->error(parseError->second, parseError->first, Error::Parser, {});
	else if (std::holds_alternative<std::nullopt_t>(data))
		emit q->failed(status, data, {});  // only pass as failed without data if any other error does not match
	else {  // no errors, completed!
		emit q->succeeded(status, data, {});
		retryDelay = -1;
	}

	if (retryDelay == 0) {
		retryDelay = -1;
		_q_retryReply();
	} else if (retryDelay > 0) {
		auto sTimer = new QTimer{q};
		sTimer->setSingleShot(true);
		sTimer->setTimerType(Qt::PreciseTimer);
		sTimer->setInterval(retryDelay);
		connect(sTimer, &QTimer::timeout,
				this, &RestReplyPrivate::_q_retryReply);
		QObject::connect(sTimer, &QTimer::timeout,
						 sTimer, &QTimer::deleteLater);
		retryDelay = -1;
		sTimer->start();
	} else if (autoDelete)
		q->deleteLater();
}

void RestReplyPrivate::_q_retryReply()
{
	auto nam = networkReply->manager();
	auto request = networkReply->request();
	auto verb = request.attribute(QNetworkRequest::CustomVerbAttribute, RestClass::GetVerb).toByteArray();
	auto body = networkReply->property(PropertyBuffer).toByteArray();

	networkReply->deleteLater();
	networkReply = compatSend(nam, request, verb, body);
	connectReply();
}

#ifndef QT_NO_SSL
void RestReplyPrivate::_q_handleSslErrors(const QList<QSslError> &errors)
{
	Q_Q(RestReply);
	bool ignore = false;
	emit q->sslErrors(errors, ignore);
	if (ignore)
		networkReply->ignoreSslErrors(errors);
}
#endif

#include "moc_restreply.cpp"
