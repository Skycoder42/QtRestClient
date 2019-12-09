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
using namespace std::chrono;
using namespace std::chrono_literals;

Q_LOGGING_CATEGORY(QtRestClient::logReply, "qt.restclient.RestReply")

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
	}, callbackType());
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

#ifdef QT_RESTCLIENT_USE_ASYNC
RestReply *RestReply::makeAsync(QThreadPool *threadPool)
{
	Q_D(RestReply);
	d->asyncPool = threadPool;
	Q_EMIT asyncChanged(d->asyncPool, {});
	return this;
}
#endif

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

#ifdef QT_RESTCLIENT_USE_ASYNC
bool RestReply::isAsync() const
{
	Q_D(const RestReply);
	return d->asyncPool;
}
#endif

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
	d->retryDelay = 0ms;
}

void RestReply::retryAfter(std::chrono::milliseconds mSecs)
{
	Q_D(RestReply);
	d->retryDelay = std::move(mSecs);
}

void RestReply::setAutoDelete(bool autoDelete)
{
	Q_D(RestReply);
	if (d->autoDelete == autoDelete)
		return;

	d->autoDelete = autoDelete;
	Q_EMIT autoDeleteChanged(autoDelete, {});
}

void RestReply::setAllowEmptyReplies(bool allowEmptyReplies)
{
	Q_D(RestReply);
	if (d->allowEmptyReplies == allowEmptyReplies)
		return;

	d->allowEmptyReplies = allowEmptyReplies;
	Q_EMIT allowEmptyRepliesChanged(d->allowEmptyReplies, {});
}

#ifdef QT_RESTCLIENT_USE_ASYNC
void RestReply::setAsync(bool async)
{
	Q_D(RestReply);
	if (static_cast<bool>(d->asyncPool) == async)
		return;

	if (async)
		makeAsync(QThreadPool::globalInstance());
	else
		makeAsync(nullptr);
}
#endif

RestReply::RestReply(RestReplyPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{
	connect(this, &RestReply::succeeded,
			this, &RestReply::completed,
			Qt::DirectConnection);
	connect(this, &RestReply::failed,
			this, &RestReply::completed,
			Qt::DirectConnection);
}

Qt::ConnectionType RestReply::callbackType() const
{
#ifdef QT_RESTCLIENT_USE_ASYNC
	Q_D(const RestReply);
	return d->asyncPool ? Qt::DirectConnection : Qt::AutoConnection;
#else
	return Qt::AutoConnection;
#endif
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

RestReplyPrivate::RestReplyPrivate()
{
	setAutoDelete(false);
}

void RestReplyPrivate::connectReply()
{
	Q_Q(RestReply);
	connect(networkReply, &QNetworkReply::finished,
			this, &RestReplyPrivate::_q_replyFinished);

	// forward some signals
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
}

void RestReplyPrivate::_q_replyFinished()
{
#ifdef QT_RESTCLIENT_USE_ASYNC
	if (asyncPool)
		asyncPool->start(this);
	else
#endif
		run();
}

void RestReplyPrivate::_q_retryReply()
{
	auto nam = networkReply->manager();
	auto request = networkReply->request();
	auto verb = request.attribute(QNetworkRequest::CustomVerbAttribute, RestClass::GetVerb).toByteArray();
	auto body = networkReply->property(PropertyBuffer).toByteArray();

	qCDebug(logReply) << "Retrying request with HTTP-Verb:"
					  << verb.constData();

	networkReply->deleteLater();
	networkReply = compatSend(nam, request, verb, body);
	connectReply();
}

#ifndef QT_NO_SSL
void RestReplyPrivate::_q_handleSslErrors(const QList<QSslError> &errors)
{
	Q_Q(RestReply);
	bool ignore = false;
	Q_EMIT q->sslErrors(errors, ignore);
	if (ignore)
		networkReply->ignoreSslErrors(errors);
}

void RestReplyPrivate::run()
{
	Q_Q(RestReply);

	if (!networkReply)
		return;

	retryDelay = -1ms;
	const auto status = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	auto contentType = networkReply->header(QNetworkRequest::ContentTypeHeader).toByteArray().trimmed();
	const auto contentLength = networkReply->header(QNetworkRequest::ContentLengthHeader).toInt();

	qCDebug(logReply) << "Received reply with status" << status
					  << "and content of type" << contentType
					  << "with length" << contentLength;

	DataType data{std::nullopt};
	std::optional<std::pair<int, QString>> parseError = std::nullopt;

	// verify content type
	if (const auto cList = contentType.split(';'); cList.size() > 1) {
		contentType = cList.first().trimmed();
		for (auto i = 1; i < cList.size(); ++i) {
			auto args = cList[i].trimmed().split('=');
			if (args.size() == 2 && args[0] == "charset") {
				if (args[1].toLower() != "utf-8") {
					parseError = std::make_pair(-1, QStringLiteral("Unsupported charset: %1").arg(QString::fromUtf8(args[1])));
					break;
				}
			} else
				qCWarning(logReply) << "Unknown content type directive:" << args[0];
		}
	}

	if (parseError) {
		// means content type is invalid -> do nothing, but is here to skip the rest
	} else if (contentLength == 0 && (status == 204 || status >= 300 || allowEmptyReplies)) {  // 204 = NO_CONTENT
		// ok, nothing to do, but is here to skip the rest
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
		Q_EMIT q->failed(status, data, {});
	else if (networkReply->error() != QNetworkReply::NoError)  // next: check normal network errors
		Q_EMIT q->error(networkReply->errorString(), networkReply->error(), Error::Network, {});
	else if (parseError)  {// next: parsing errors
		Q_EMIT q->error(parseError->second, parseError->first, Error::Parser, {});
	} else if (status >= 300 && std::holds_alternative<std::nullopt_t>(data))
		Q_EMIT q->failed(status, data, {});  // only pass as failed without data if any other error does not match
	else {  // no errors, completed!
		Q_EMIT q->succeeded(status, data, {});
		retryDelay = -1ms;
	}

	if (retryDelay == 0ms) {
		retryDelay = -1ms;
		QMetaObject::invokeMethod(q, "_q_retryReply");
	} else if (retryDelay > 0ms) {
		qCDebug(logReply) << "Retrying request in"
						  << retryDelay.count()
						  << "milliseconds";
		auto sTimer = new QTimer{};
		sTimer->setSingleShot(true);
		sTimer->setTimerType(Qt::PreciseTimer);
		sTimer->setInterval(retryDelay);
		sTimer->moveToThread(q->thread());
		connect(sTimer, &QTimer::timeout,
				this, &RestReplyPrivate::_q_retryReply);
		QObject::connect(sTimer, &QTimer::timeout,
						 sTimer, &QTimer::deleteLater);
		retryDelay = -1ms;
		QMetaObject::invokeMethod(sTimer, "start");
	} else if (autoDelete)
		QMetaObject::invokeMethod(q, "deleteLater");
}
#endif

#include "moc_restreply.cpp"
