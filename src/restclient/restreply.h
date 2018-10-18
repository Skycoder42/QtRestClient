#ifndef QTRESTCLIENT_RESTREPLY_H
#define QTRESTCLIENT_RESTREPLY_H

#include "QtRestClient/qtrestclient_global.h"

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

public:
	//! Defines the different possible error types
	enum ErrorType {
		//default error types
		NetworkError,//!< Indicates a network error, i.e. no internet
		JsonParseError,//!< Indicates that parsing the received JSON failed
		FailureError,//!< Indicates that the server sent a failure for the request

		//extended error types
		DeserializationError//!< Indicates that deserializing the received JSON to the target object failed. **Generic replies only!**
	};
	Q_ENUM(ErrorType)

	//! Creates a new reply based on a network reply
	RestReply(QNetworkReply *networkReply, QObject *parent = nullptr);

	//! Set a handler to be called if the request succeeded
	RestReply *onSucceeded(const std::function<void(int, QJsonObject)> &handler);
	//! @copybrief onSucceeded(const std::function<void(int, QJsonObject)>&)
	RestReply *onSucceeded(QObject *scope, const std::function<void(int, QJsonObject)> &handler);
	//! @copydoc onSucceeded(const std::function<void(int, QJsonObject)>&)
	RestReply *onSucceeded(const std::function<void(int, QJsonArray)> &handler);
	//! @copydoc onSucceeded(QObject *, const std::function<void(int, QJsonObject)>&)
	RestReply *onSucceeded(QObject *scope, const std::function<void(int, QJsonArray)> &handler);
	//! Set a handler to be called if the request failed
	RestReply *onFailed(const std::function<void(int, QJsonObject)> &handler);
	//! @copybrief onFailed(const std::function<void(int, QJsonObject)>&)
	RestReply *onFailed(QObject *scope, const std::function<void(int, QJsonObject)> &handler);
	//! @copydoc onFailed(const std::function<void(int, QJsonObject)>&)
	RestReply *onFailed(const std::function<void(int, QJsonArray)> &handler);
	//! @copydoc onFailed(QObject *, const std::function<void(int, QJsonObject)>&)
	RestReply *onFailed(QObject *scope, const std::function<void(int, QJsonArray)> &handler);
	//! Set a handler to be called when the request was completed, regardless of success or failure
	RestReply *onCompleted(const std::function<void(int)> &handler);
	//! @copybrief onCompleted(const std::function<void(int)>&)
	RestReply *onCompleted(QObject *scope, const std::function<void(int)> &handler);
	//! Set a handler to be called if a network error or json parse error occures
	RestReply *onError(const std::function<void(QString, int, ErrorType)> &handler);
	//! @copybrief onError(const std::function<void(QString, int, ErrorType)>&)
	RestReply *onError(QObject *scope, const std::function<void(QString, int, ErrorType)> &handler);
	//! Set a handler to be called if the request did not succeed
	RestReply *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
						   const std::function<QString(QJsonObject, int)> &failureTransformer = {});
	//! @copybrief onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, ErrorType)> &handler,
						   const std::function<QString(QJsonObject, int)> &failureTransformer = {});
	//! @copydoc onAllErrors(const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	RestReply *onAllErrors(const std::function<void(QString, int, ErrorType)> &handler,
						   const std::function<QString(QJsonArray, int)> &failureTransformer);
	//! @copydoc onAllErrors(QObject *, const std::function<void(QString, int, ErrorType)>&, const std::function<QString(QJsonObject, int)>&)
	RestReply *onAllErrors(QObject *scope,
						   const std::function<void(QString, int, ErrorType)> &handler,
						   const std::function<QString(QJsonArray, int)> &failureTransformer);

	//! @writeAcFn{RestReply::autoDelete}
	Q_INVOKABLE inline RestReply *disableAutoDelete() {
		setAutoDelete(false);
		return this;
	}

	//! @readAcFn{RestReply::autoDelete}
	bool autoDelete() const;

	//! Returns the network reply associated with the rest reply
	QNetworkReply *networkReply() const;

	RestReplyAwaitable await();

public Q_SLOTS:
	//! Aborts the request by calling QNetworkReply::abort
	void abort();
	//! Tries to make the same request again, and reuses this rest reply
	void retry();
	//! Tries to make the same request again after a delay, and reuses this rest reply
	void retryAfter(int mSecs);

	//! @writeAcFn{RestReply::autoDelete}
	void setAutoDelete(bool autoDelete);

Q_SIGNALS:
	//! Is emitted when the request completed, i.e. succeeded or failed
	void completed(int httpStatus, const QJsonValue &reply, QPrivateSignal);
	//! Is emitted when the request succeeded
	void succeeded(int httpStatus, const QJsonValue &reply, QPrivateSignal);
	//! Is emitted when the request failed
	void failed(int httpStatus, const QJsonValue &reason, QPrivateSignal);
	//! Is emitted when a network or json parse error occured
	void error(const QString &errorString, int error, ErrorType errorType, QPrivateSignal);

	//! Forwards QNetworkReply::error(QNetworkReply::NetworkError)
	void networkError(QNetworkReply::NetworkError error);
	//! Forwards QNetworkReply::sslErrors
	void sslErrors(const QList<QSslError> &errors, bool &ignoreErrors);

	//! Forwards QNetworkReply::downloadProgress
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	//! Forwards QNetworkReply::uploadProgress
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

	//! @notifyAcFn{RestReply::autoDelete}
	void autoDeleteChanged(bool autoDelete, QPrivateSignal);

protected:
	//! @private
	static QByteArray jsonTypeName(QJsonValue::Type type);

private:
	RestReplyPrivate *d;
};

}

#endif // QTRESTCLIENT_RESTREPLY_H
