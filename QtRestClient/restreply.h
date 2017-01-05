#ifndef RESTREPLY_H
#define RESTREPLY_H

#include "qtrestclient_global.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <functional>

namespace QtRestClient {

class RestReplyPrivate;
class QTRESTCLIENTSHARED_EXPORT RestReply : public QObject
{
	Q_OBJECT
	friend class RestReplyPrivate;

	Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)

public:
	enum ErrorType {
		NetworkError,
		JsonParseError
	};
	Q_ENUM(ErrorType)

	RestReply(QNetworkReply *networkReply, QObject *parent = nullptr);

	RestReply *onSucceeded(std::function<void(RestReply*, int, QJsonObject)> handler);
	RestReply *onSucceeded(std::function<void(RestReply*, int, QJsonArray)> handler);
	RestReply *onFailed(std::function<void(RestReply*, int, QJsonObject)> handler);
	RestReply *onFailed(std::function<void(RestReply*, int, QJsonArray)> handler);
	RestReply *onError(std::function<void(RestReply*, QString, int, ErrorType)> handler);

	inline RestReply *enableAutoDelete() {
		setAutoDelete(true);
		return this;
	}

	bool autoDelete() const;

	//TODO add getters usw

public slots:
	void abort();
	void retry();
	void retryAfter(int mSecs);

	void setAutoDelete(bool autoDelete);

signals:
	void succeeded(int httpStatus, const QJsonValue &reply, QPrivateSignal);
	void failed(int httpStatus, const QJsonValue &reason, QPrivateSignal);
	void error(const QString &errorString, int error, ErrorType errorType, QPrivateSignal);

	void networkError(QNetworkReply::NetworkError error);
	void sslErrors(const QList<QSslError> &errors, bool &ignoreErrors);

	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

	void autoDeleteChanged(bool autoDelete, QPrivateSignal);

private:
	RestReplyPrivate *d_ptr;
};

}

#endif // RESTREPLY_H
