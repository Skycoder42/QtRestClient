#ifndef RESTREPLY_P_H
#define RESTREPLY_P_H

#include "restreply.h"

namespace QtRestClient {

class RestReplyPrivate : public QObject
{
	Q_OBJECT

public:
	static const QByteArray PropertyVerb;
	static const QByteArray PropertyBuffer;

	static QIODevice *cloneDevice(QIODevice *device);
	static QNetworkReply *compatSend(QNetworkAccessManager *nam, QNetworkRequest request, QByteArray verb, QIODevice *buffer);

	QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> networkReply;
	bool autoDelete;
	int retryDelay;

	RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr);

	void connectReply(QNetworkReply *reply);

public slots:
	void replyFinished();
	void handleSslErrors(const QList<QSslError> &errors);

private slots:
	void retryReply();

private:
	RestReply *q_ptr;
};

}

#endif // RESTREPLY_P_H
