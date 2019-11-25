#include "restreply.h"
#include "restreply_p.h"
#include "restreplyawaitable.h"
#include "restclass.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>

using namespace QtRestClient;


RestReply::RestReply(QNetworkReply *networkReply, QObject *parent) :
	QObject{parent},
	d{new RestReplyPrivate{networkReply, this}}
{
	d->connectReply(networkReply);
}

RestReply *RestReply::onSucceeded(const std::function<void (int, QJsonObject)> &handler)
{
	return onSucceeded(this, handler);
}

RestReply *RestReply::onSucceeded(QObject *scope, const std::function<void (int, QJsonObject)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, scope, [handler](int code, const QJsonValue &value){
		handler(code, value.toObject());
	});
	return this;
}

RestReply *RestReply::onSucceeded(const std::function<void (int, QJsonArray)> &handler)
{
	return onSucceeded(this, handler);
}

RestReply *RestReply::onSucceeded(QObject *scope, const std::function<void (int, QJsonArray)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, scope, [handler](int code, const QJsonValue &value){
		handler(code, value.toArray());
	});
	return this;
}

RestReply *RestReply::onSucceeded(const std::function<void (int)> &handler)
{
	return onSucceeded(this, handler);
}

RestReply *RestReply::onSucceeded(QObject *scope, const std::function<void (int)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, scope, [handler](int code, const QJsonValue &){
		handler(code);
	});
	return this;
}

RestReply *RestReply::onFailed(const std::function<void (int, QJsonObject)> &handler)
{
	return onFailed(this, handler);
}

RestReply *RestReply::onFailed(QObject *scope, const std::function<void (int, QJsonObject)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, scope, [handler](int code, const QJsonValue &value){
		handler(code, value.toObject());
	});
	return this;
}

RestReply *RestReply::onFailed(const std::function<void (int, QJsonArray)> &handler)
{
	return onFailed(this, handler);
}

RestReply *RestReply::onFailed(QObject *scope, const std::function<void (int, QJsonArray)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, scope, [handler](int code, const QJsonValue &value){
		handler(code, value.toArray());
	});
	return this;
}

RestReply *RestReply::onFailed(const std::function<void (int)> &handler)
{
	return onFailed(this, handler);
}

RestReply *RestReply::onFailed(QObject *scope, const std::function<void (int)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, scope, [handler](int code, const QJsonValue &){
		handler(code);
	});
	return this;
}

RestReply *RestReply::onCompleted(const std::function<void (int)> &handler)
{
	return onCompleted(this, handler);
}

RestReply *RestReply::onCompleted(QObject *scope, const std::function<void (int)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::completed, scope, [handler](int code, const QJsonValue &){
		handler(code);
	});
	return this;
}

RestReply *RestReply::onError(const std::function<void (QString, int, ErrorType)> &handler)
{
	return onError(this, handler);
}

RestReply *RestReply::onError(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::error, scope, [handler](const QString &errorString, int error, ErrorType type){
		handler(errorString, error, type);
	});
	return this;
}

RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::ErrorType)> &handler, const std::function<QString (QJsonObject, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler, const std::function<QString (QJsonObject, int)> &failureTransformer)
{
	this->onFailed(scope, [handler, failureTransformer](int code, const QJsonObject &obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
	});
	this->onError(handler);
	return this;
}

RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::ErrorType)> &handler, const std::function<QString (QJsonArray, int)> &failureTransformer)
{
	return onAllErrors(this, handler, failureTransformer);
}

RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler, const std::function<QString (QJsonArray, int)> &failureTransformer)
{
	this->onFailed(scope, [handler, failureTransformer](int code, const QJsonArray &array){
		if(failureTransformer)
			handler(failureTransformer(array, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
	});
	this->onError(handler);
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

	//read json first to allow data for certain network fails
	auto readData = networkReply->readAll();
	QJsonValue jValue;
	QJsonParseError jError {0, QJsonParseError::NoError};
	// always allow empty replies for NO_CONTENT, errors and if explicitly allowed
	if (readData.isEmpty() && (status == 204 || status >= 300 || allowEmptyReplies))  // 204 = NO_CONTENT
		jValue = QJsonValue::Null;  // set to NULL to indicate and empty body
	else {
		auto jDoc = QJsonDocument::fromJson(readData, &jError);
		if(jDoc.isObject())
			jValue = jDoc.object();
		else if(jDoc.isArray())
			jValue = jDoc.array();
	}

	//check "http errors", because they can have data, but only if json is valid
	if(jError.error == QJsonParseError::NoError && status >= 300)//first: status code error + valid json
		emit q->failed(status, jValue, {});
	else if(networkReply->error() != QNetworkReply::NoError)//next: check normal network errors
		emit q->error(networkReply->errorString(), networkReply->error(), RestReply::NetworkError, {});
	else if(jError.error != QJsonParseError::NoError) //next: json errors
		emit q->error(jError.errorString(), jError.error, RestReply::JsonParseError, {});
	else {//no errors, completed!
		emit q->succeeded(status, jValue, {});
		retryDelay = -1;
	}

	if(retryDelay == 0) {
		retryDelay = -1;
		retryReply();
	} else if(retryDelay > 0) {
		QTimer::singleShot(retryDelay, Qt::PreciseTimer, this, &RestReplyPrivate::retryReply);
		retryDelay = -1;
	} else if(autoDelete)
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
