#include "restreply.h"
#include "restreply_p.h"

#include <QJsonDocument>

using namespace QtRestClient;

#define d d_ptr

RestReply::RestReply(QNetworkReply *networkReply) :
	QObject(),
	d_ptr(new RestReplyPrivate(networkReply, this))
{
	connect(networkReply, &QNetworkReply::finished,
			d, &RestReplyPrivate::replyFinished);

	//forward some signals
	connect(networkReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
			this, &RestReply::networkError);
	connect(networkReply, &QNetworkReply::sslErrors,
			d, &RestReplyPrivate::handleSslErrors);
	connect(networkReply, &QNetworkReply::downloadProgress,
			this, &RestReply::downloadProgress);
	connect(networkReply, &QNetworkReply::uploadProgress,
			this, &RestReply::uploadProgress);
}

RestReply &RestReply::onSucceeded(std::function<void (RestReply *, int, QJsonObject)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		handler(this, code, value.toObject());
	});
	return *this;
}

RestReply &RestReply::onSucceeded(std::function<void (RestReply *, int, QJsonArray)> handler)
{
	connect(this, &RestReply::succeeded, this, [=](int code, const QJsonValue &value){
		handler(this, code, value.toArray());
	});
	return *this;
}

RestReply &RestReply::onFailed(std::function<void (RestReply*, int, QJsonObject)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		handler(this, code, value.toObject());
	});
	return *this;
}

RestReply &RestReply::onFailed(std::function<void (RestReply*, int, QJsonArray)> handler)
{
	connect(this, &RestReply::failed, this, [=](int code, const QJsonValue &value){
		handler(this, code, value.toArray());
	});
	return *this;
}

RestReply &RestReply::onError(std::function<void (RestReply*, QString, int, ErrorType)> handler)
{
	connect(this, &RestReply::error, this, [=](QString errorString, int error, ErrorType type){
		handler(this, errorString, error, type);
	});
	return *this;
}

bool RestReply::autoDelete() const
{
	return d->autoDelete;
}

void RestReply::abort()
{
	d->networkReply->abort();
}

void RestReply::retry()
{
	d->retry = true;
}

void RestReply::setAutoDelete(bool autoDelete)
{
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	emit autoDeleteChanged(autoDelete, {});
}

// ------------- Private Implementation -------------

RestReplyPrivate::RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr) :
	QObject(q_ptr),
	networkReply(networkReply),
	autoDelete(false),
	retry(false),
	q_ptr(q_ptr)
{}

void RestReplyPrivate::replyFinished()
{
	retry = false;
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
				retry = false;
			}
		}
	}

	if(retry) {
		retry = false;
		//TODO retry!
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
