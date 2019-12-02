#ifndef QTRESTCLIENT_RESTREPLY_H
#define QTRESTCLIENT_RESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/metacomponent.h"

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtNetwork/qnetworkreply.h>
#include <functional>

namespace QtRestClient {

class RestReplyPrivate;
class RestReplyAwaitable;
class QmlGenericRestReply; //needed for QML bindings
//! A class to handle replies for JSON requests
class Q_RESTCLIENT_EXPORT RestReply : public QObject
{
	Q_OBJECT
	friend class RestReplyPrivate;

	//! Speciefies, whether the reply should be automatically deleted
	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)
	//! Speciefies, whether empty rest replies are allowed
	Q_PROPERTY(bool allowEmptyReplies READ allowsEmptyReplies WRITE setAllowEmptyReplies NOTIFY allowEmptyRepliesChanged REVISION 2)

public:
	using DataType = __private::__binder::DataType;

	//! Defines the different possible error types
	enum ErrorType {
		//default error types
		NetworkError,  //!< Indicates a network error, i.e. no internet
		ParseError,  //!< Indicates that parsing the received JSON or CBOR data failed
		FailureError,  //!< Indicates that the server sent a failure for the request

		//extended error types
		DeserializationError  //!< Indicates that deserializing the received data to the target object failed. **Generic replies only!**
	};
	Q_ENUM(ErrorType)

	//! Creates a new reply based on a network reply
	RestReply(QNetworkReply *networkReply, QObject *parent = nullptr);

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
	RestReply *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copybrief onError(const std::function<void(QString, int, ErrorType)>&)
	RestReply *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);

	//! Set a handler to be called if the request did not succeed
	RestReply *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copybrief onAllErrors(const std::function<void(QString, int, ErrorType)>&)
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, ErrorType)> &handler);
	//! @copydoc onAllErrors(const std::function<void(QString, int, ErrorType)>&)
	template <typename TFn>
	RestReply *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
						   TFn &&failureTransformer);
	//! @copydoc onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&)
	template <typename TFn>
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, ErrorType)> &handler,
						   TFn &&failureTransformer);

	//! @writeAcFn{RestReply::autoDelete}
	Q_INVOKABLE inline RestReply *disableAutoDelete() {
		setAutoDelete(false);
		return this;
	}

	//! @readAcFn{RestReply::autoDelete}
	bool autoDelete() const;
	//! @readAcFn{RestReply::allowEmptyReplies}
	bool allowsEmptyReplies() const;

	//! Returns the network reply associated with the rest reply
	QNetworkReply *networkReply() const;

	//! Returns an awaitable object for this reply
	RestReplyAwaitable awaitable();

public Q_SLOTS:
	//! Aborts the request by calling QNetworkReply::abort
	void abort();
	//! Tries to make the same request again, and reuses this rest reply
	void retry();
	//! Tries to make the same request again after a delay, and reuses this rest reply
	void retryAfter(int mSecs);

	//! @writeAcFn{RestReply::autoDelete}
	void setAutoDelete(bool autoDelete);
	//! @writeAcFn{RestReply::allowEmptyReplies}
	Q_REVISION(2) void setAllowEmptyReplies(bool allowEmptyReplies);

Q_SIGNALS:
	//! Is emitted when the request completed, i.e. succeeded or failed
	void completed(int httpStatus, const DataType &reply, QPrivateSignal);
	//! Is emitted when the request succeeded
	void succeeded(int httpStatus, const DataType &reply, QPrivateSignal);
	//! Is emitted when the request failed
	void failed(int httpStatus, const DataType &reason, QPrivateSignal);
	//! Is emitted when a network or json parse error occured
	void error(const QString &errorString, int error, ErrorType errorType, QPrivateSignal);

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
	Q_REVISION(2) void allowEmptyRepliesChanged(bool allowEmptyReplies, QPrivateSignal);

protected:
	//! @private
	static QByteArray jsonTypeName(QJsonValue::Type type);

private:
	RestReplyPrivate *d;
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
			scope, __private::bindCallback(std::forward<TFn>(handler)));
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
			scope, __private::bindCallback(std::forward<TFn>(handler)));
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
			scope, __private::bindCallback(std::forward<TFn>(handler)));
	return this;
}

template<typename TFn>
RestReply *RestReply::onAllErrors(const std::function<void (QString, int, RestReply::ErrorType)> &handler, TFn &&failureTransformer)
{
	return onAllErrors(this, handler, std::forward<TFn>(failureTransformer));
}

template<typename TFn>
RestReply *RestReply::onAllErrors(QObject *scope, const std::function<void (QString, int, RestReply::ErrorType)> &handler, TFn &&failureTransformer)
{
	this->onFailed(scope, __private::bindCallback(handler, std::forward<TFn>(failureTransformer), FailureError));
	this->onError(scope, handler);
	return this;
}

}

#endif // QTRESTCLIENT_RESTREPLY_H
