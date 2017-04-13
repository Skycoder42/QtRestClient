#ifndef QTRESTCLIENT_RESTREPLY_P_H
#define QTRESTCLIENT_RESTREPLY_P_H

#include "restreply.h"

#include <QtCore/QPointer>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestReplyPrivate : public QObject
{
	Q_OBJECT

public:
	static const QByteArray PropertyVerb;
	static const QByteArray PropertyBuffer;

	static QIODevice *cloneDevice(QIODevice *device);
	static QNetworkReply *compatSend(QNetworkAccessManager *nam, QNetworkRequest request, QByteArray verb, QIODevice *buffer);

	QPointer<QNetworkReply> networkReply;
	bool autoDelete;
	int retryDelay;

	RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr);
	~RestReplyPrivate();

	void connectReply(QNetworkReply *reply);

public Q_SLOTS:
	void replyFinished();
	void handleSslErrors(const QList<QSslError> &errors);

private Q_SLOTS:
	void retryReply();

private:
	RestReply *q;
};

}

#endif // QTRESTCLIENT_RESTREPLY_P_H
