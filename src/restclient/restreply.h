#ifndef QTRESTCLIENT_RESTREPLY_H
#define QTRESTCLIENT_RESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/qtrestclient_helpertypes.h"

#include <functional>
#include <chrono>

#ifdef QT_RESTCLIENT_USE_ASYNC
#include <QtCore/QThreadPool>
#include <QtCore/QFuture>
#endif

#include <QtNetwork/qnetworkreply.h>

namespace QtRestClient {

class RestReplyPrivate;
class RestReplyAwaitable;
class QmlGenericRestReply; //needed for QML bindings
//! A class to handle replies for JSON requests
class Q_RESTCLIENT_EXPORT RestReply : public QObject
{
	Q_OBJECT

	//! Speciefies, whether the reply should be automatically deleted
	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)
	//! Speciefies, whether empty rest replies are allowed
	Q_PROPERTY(bool allowEmptyReplies READ allowsEmptyReplies WRITE setAllowEmptyReplies NOTIFY allowEmptyRepliesChanged)
#ifdef QT_RESTCLIENT_USE_ASYNC
	Q_PROPERTY(bool async READ isAsync WRITE setAsync NOTIFY asyncChanged)
#endif

public:
	using DataType = __private::__binder::DataType;

	//! Defines the different possible error types
	enum class Error {
		//default error types
		Network,  //!< Indicates a network error, i.e. no internet
		Parser,  //!< Indicates that parsing the received JSON or CBOR data failed
		Failure,  //!< Indicates that the server sent a failure for the request

		//extended error types
		Deserialization  //!< Indicates that deserializing the received data to the target object failed. **Generic replies only!**
	};
	Q_ENUM(Error)

	//! Creates a new reply based on a network reply
	RestReply(QNetworkReply *networkReply, QObject *parent = nullptr);
#ifdef QT_RESTCLIENT_USE_ASYNC
	RestReply(const QFuture<QNetworkReply*> &networkReplyFuture, QObject *parent = nullptr);
	RestReply(QNetworkReply *networkReply, QThreadPool *asyncPool, QObject *parent = nullptr);
	RestReply(const QFuture<QNetworkReply*> &networkReplyFuture, QThreadPool *asyncPool, QObject *parent = nullptr);
#endif
	~RestReply() override;

	// TODO add allowed signatures to doc

	//! Set a handler to be called if the request succeeded
	template <typename TFn>
	RestReply *onSucceeded(TFn &&handler);
	//! @copybrief onSucceeded(TFn&&)
	template <typename TFn>
	RestReply *onSucceeded(QObject *scope, TFn &&handler);
	//! Set a handler to be called if the request failed
	template <typename TFn>
	RestReply *onFailed(TFn &&handler);
	//! @copybrief onFailed(TFn&&)
	template <typename TFn>
	RestReply *onFailed(QObject *scope, TFn &&handler);
	//! Set a handler to be called when the request was completed, regardless of success or failure
	template <typename TFn>
	RestReply *onCompleted(TFn &&handler);
	//! @copybrief onCompleted(TFn&&)
	template <typename TFn>
	RestReply *onCompleted(QObject *scope, TFn &&handler);

	//! Set a handler to be called if a network error or json parse error occures
	RestReply *onError(std::function<void(QString, int, Error)> handler);
	//! @copybrief onError(const std::function<void(QString, int, ErrorType)>&)
	RestReply *onError(QObject *scope, std::function<void(QString, int, Error)> handler);

	//! Set a handler to be called if the request did not succeed
	RestReply *onAllErrors(const std::function<void(QString, int, Error)> &handler);
	//! @copybrief onAllErrors(const std::function<void(QString, int, ErrorType)>&)
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, Error)> &handler);
	//! @copydoc onAllErrors(const std::function<void(QString, int, ErrorType)>&)
	template <typename TFn>
	RestReply *onAllErrors(const std::function<void(QString, int, Error)> &handler,
						   TFn &&failureTransformer);
	//! @copydoc onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&)
	template <typename TFn>
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, Error)> &handler,
						   TFn &&failureTransformer);

#ifdef QT_RESTCLIENT_USE_ASYNC
	Q_INVOKABLE RestReply *makeAsync(QThreadPool *threadPool = QThreadPool::globalInstance());
#endif
	//! @writeAcFn{RestReply::autoDelete}
	Q_INVOKABLE inline RestReply *disableAutoDelete() {
		setAutoDelete(false);
		return this;
	}

	//! @readAcFn{RestReply::autoDelete}
	bool autoDelete() const;
	//! @readAcFn{RestReply::allowEmptyReplies}
	bool allowsEmptyReplies() const;
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @readAcFn{RestReply::async}
	bool isAsync() const;
#endif

	//! Returns the network reply associated with the rest reply
	Q_INVOKABLE QNetworkReply *networkReply() const;

	//! Returns an awaitable object for this reply
	RestReplyAwaitable awaitable();

public Q_SLOTS:
	//! Aborts the request by calling QNetworkReply::abort
	void abort();
	//! Tries to make the same request again, and reuses this rest reply
	void retry();
	//! Tries to make the same request again after a delay, and reuses this rest reply
	void retryAfter(std::chrono::milliseconds mSecs);

	//! @writeAcFn{RestReply::autoDelete}
	void setAutoDelete(bool autoDelete);
	//! @writeAcFn{RestReply::allowEmptyReplies}
	void setAllowEmptyReplies(bool allowEmptyReplies);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @writeAcFn{RestReply::async}
	void setAsync(bool async);
#endif

Q_SIGNALS:
	//! Is emitted when the request completed, i.e. succeeded or failed
	void completed(int httpStatus, const DataType &reply, QPrivateSignal);
	//! Is emitted when the request succeeded
	void succeeded(int httpStatus, const DataType &reply, QPrivateSignal);
	//! Is emitted when the request failed
	void failed(int httpStatus, const DataType &reason, QPrivateSignal);
	//! Is emitted when a network or json parse error occured
	void error(const QString &errorString, int error, Error errorType, QPrivateSignal);

	//! Forwards QNetworkReply::error(QNetworkReply::NetworkError)
	void networkError(QNetworkReply::NetworkError error);
#ifndef QT_NO_SSL
	//! Forwards QNetworkReply::sslErrors
	void sslErrors(const QList<QSslError> &errors, bool &ignoreErrors);
#endif

	//! Forwards QNetworkReply::downloadProgress
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	//! Forwards QNetworkReply::uploadProgress
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

	//! @notifyAcFn{RestReply::autoDelete}
	void autoDeleteChanged(bool autoDelete, QPrivateSignal);
	//! @notifyAcFn{RestReply::allowEmptyReplies}
	void allowEmptyRepliesChanged(bool allowEmptyReplies, QPrivateSignal);
#ifdef QT_RESTCLIENT_USE_ASYNC
	//! @notifyAcFn{RestReply::async}
	void asyncChanged(bool async, QPrivateSignal);
#endif

protected:
	//! @private
	RestReply(RestReplyPrivate &dd, QObject *parent = nullptr);

private:
	Q_DECLARE_PRIVATE(RestReply)

	Q_PRIVATE_SLOT(d_func(), void _q_replyFinished())
	Q_PRIVATE_SLOT(d_func(), void _q_retryReply())
#ifndef QT_NO_SSL
	Q_PRIVATE_SLOT(d_func(), void _q_handleSslErrors(const QList<QSslError> &))
#endif

	Qt::ConnectionType callbackType() const;
};

template<typename TFn>
RestReply *RestReply::onSucceeded(TFn &&handler)
{
	return onSucceeded(this, std::forward<TFn>(handler));
}

template<typename TFn>
RestReply *RestReply::onSucceeded(QObject *scope, TFn &&handler)
{
	connect(this, &RestReply::succeeded,
			scope, __private::bindCallback(std::forward<TFn>(handler)),
			callbackType());
	return this;
}

template<typename TFn>
RestReply *RestReply::onFailed(TFn &&handler)
{
	return onFailed(this, std::forward<TFn>(handler));
}

template<typename TFn>
RestReply *RestReply::onFailed(QObject *scope, TFn &&handler)
{
	connect(this, &RestReply::failed,
			scope, __private::bindCallback(std::forward<TFn>(handler)),
			callbackType());
	return this;
}

template<typename TFn>
RestReply *RestReply::onCompleted(TFn &&handler)
{
	return onCompleted(this, std::forward<TFn>(handler));
}

template<typename TFn>
RestReply *RestReply::onCompleted(QObject *scope, TFn &&handler)
{
	connect(this, &RestReply::completed,
			scope, __private::bindCallback(std::forward<TFn>(handler)),
			callbackType());
	return this;
}

template<typename TFn>
RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::Error)> &handler, TFn &&failureTransformer)
{
	return onAllErrors(this, handler, std::forward<TFn>(failureTransformer));
}

template<typename TFn>
RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::Error)> &handler, TFn &&failureTransformer)
{
	this->onFailed(scope, __private::bindCallback(handler, std::forward<TFn>(failureTransformer), Error::Failure));
	this->onError(scope, handler);
	return this;
}

}

#endif // QTRESTCLIENT_RESTREPLY_H
