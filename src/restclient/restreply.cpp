#include "restreply.h"
#include "restreply_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>

using namespace QtRestClient;

#define d d_ptr

RestReply::RestReply(QNetworkReply *networkReply, QObject *parent) :
	QObject(parent),
	d_ptr(new RestReplyPrivate(networkReply, this))
{
	d->connectReply(networkReply);
}

RestReply *RestReply::onSucceeded(std::function<void (int, QJsonObject)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		handler(code, value.toObject());
	});
	return this;
}

RestReply *RestReply::onSucceeded(std::function<void (int, QJsonArray)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		handler(code, value.toArray());
	});
	return this;
}

RestReply *RestReply::onFailed(std::function<void (int, QJsonObject)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		handler(code, value.toObject());
	});
	return this;
}

RestReply *RestReply::onFailed(std::function<void (int, QJsonArray)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		handler(code, value.toArray());
	});
	return this;
}

RestReply *RestReply::onError(std::function<void (QString, int, ErrorType)> handler)
{
	if(!handler)
		return this;
	connect(this, &RestReply::error, this, [=](QString errorString, int error, ErrorType type){
		handler(errorString, error, type);
	});
	return this;
}

RestReply *RestReply::onAllErrors(std::function<void (QString, int, RestReply::ErrorType)> handler, std::function<QString (QJsonObject, int)> failureTransformer)
{
	this->onFailed([=](int code, QJsonObject obj){
		if(failureTransformer)
			handler(failureTransformer(obj, code), code, FailureError);
		else
			handler(QString(), code, FailureError);
	});
	this->onError(handler);
	return this;
}

RestReply *RestReply::onAllErrors(std::function<void (QString, int, RestReply::ErrorType)> handler, std::function<QString (QJsonArray, int)> failureTransformer)
{
	this->onFailed([=](int code, QJsonArray array){
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

QNetworkReply *RestReply::networkReply() const
{
	return d->networkReply.data();
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

// ------------- Private Implementation -------------

const QByteArray RestReplyPrivate::PropertyVerb("__QtRestClient_RestReplyPrivate_PropertyVerb");
const QByteArray RestReplyPrivate::PropertyBuffer("__QtRestClient_RestReplyPrivate_PropertyBuffer");

QIODevice *RestReplyPrivate::cloneDevice(QIODevice *device)
{
	if(device->isSequential())
		return nullptr;
	else {
		auto rPos = device->pos();
		device->seek(0);

		auto buffer = new QBuffer();
		buffer->setData(device->readAll());
		buffer->open(QIODevice::ReadOnly);

		device->seek(rPos);

		return buffer;
	}
}

QNetworkReply *RestReplyPrivate::compatSend(QNetworkAccessManager *nam, QNetworkRequest request, QByteArray verb, QIODevice *buffer)
{
	auto reply = nam->sendCustomRequest(request, verb, buffer);
	if(reply) {
		reply->setProperty(PropertyVerb, verb);
		if(buffer) {
			reply->setProperty(PropertyBuffer, QVariant::fromValue(buffer));
			QObject::connect(reply, &QNetworkReply::destroyed, [=](){
				buffer->close();
				buffer->deleteLater();
			});
		}
	} else if(buffer) {
		buffer->close();
		buffer->deleteLater();
	}
	return reply;
}

RestReplyPrivate::RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr) :
	QObject(q_ptr),
	networkReply(networkReply),
	autoDelete(false),
	retryDelay(-1),
	q_ptr(q_ptr)
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
			q_ptr, &RestReply::networkError);
	connect(reply, &QNetworkReply::sslErrors,
			this, &RestReplyPrivate::handleSslErrors);
	connect(reply, &QNetworkReply::downloadProgress,
			q_ptr, &RestReply::downloadProgress);
	connect(reply, &QNetworkReply::uploadProgress,
			q_ptr, &RestReply::uploadProgress);
}

void RestReplyPrivate::replyFinished()
{
	retryDelay = -1;
	//check json first to allow data for certain network fails
	auto readData = networkReply->readAll();
	QJsonParseError jError;
	auto jDoc = QJsonDocument::fromJson(readData, &jError);
	if(jError.error != QJsonParseError::NoError && !readData.isEmpty())
		emit q_ptr->error(jError.errorString(), jError.error, RestReply::JsonParseError, {});
	else {
		QJsonValue jValue;
		if(jDoc.isObject())
			jValue = jDoc.object();
		else if(jDoc.isArray())
			jValue = jDoc.array();

		//check "http errors", becaus they can have data
		auto status = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if(status >= 300)//errors begin at 300
			emit q_ptr->failed(status, jValue, {});
		else {//now network errors
			if(networkReply->error() != QNetworkReply::NoError)
				emit q_ptr->error(networkReply->errorString(), networkReply->error(), RestReply::NetworkError, {});
			else {//no errors, completed!
				emit q_ptr->succeeded(status, jValue, {});
				retryDelay = -1;
			}
		}
	}

	if(retryDelay == 0) {
		retryDelay = -1;
		retryReply();
	} else if(retryDelay > 0) {
		QTimer::singleShot(retryDelay, Qt::PreciseTimer, this, &RestReplyPrivate::retryReply);
		retryDelay = -1;
	} else if(autoDelete)
		q_ptr->deleteLater();
}

void RestReplyPrivate::handleSslErrors(const QList<QSslError> &errors)
{
	bool ignore = false;
	emit q_ptr->sslErrors(errors, ignore);
	if(ignore)
		networkReply->ignoreSslErrors(errors);
}

void RestReplyPrivate::retryReply()
{
	auto nam = networkReply->manager();
	auto request = networkReply->request();
	auto verb = networkReply->property(PropertyVerb).toByteArray();
	if(verb.isEmpty())
		verb = "GET";
	auto buffer = networkReply->property(PropertyBuffer).value<QIODevice*>();
	if(buffer)
		buffer = cloneDevice(buffer);

	networkReply->deleteLater();
	networkReply = compatSend(nam, request, verb, buffer);
	connectReply(networkReply);
}
