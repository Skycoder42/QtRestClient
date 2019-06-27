#ifndef QTRESTCLIENT_RESTREPLY_P_H
#define QTRESTCLIENT_RESTREPLY_P_H

#include "restreply.h"

#include <QtCore/QPointer>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestReplyPrivate : public QObject
{
	Q_OBJECT

public:
	static const QByteArray PropertyBuffer;

	static QNetworkReply *compatSend(QNetworkAccessManager *nam, const QNetworkRequest &request, const QByteArray &verb, const QByteArray &body);

	QPointer<QNetworkReply> networkReply;
	bool autoDelete = true;
	int retryDelay = -1;

	RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr);
	~RestReplyPrivate() override;

	void connectReply(QNetworkReply *reply);

public Q_SLOTS:
	void replyFinished();
#ifndef QT_NO_SSL
	void handleSslErrors(const QList<QSslError> &errors);
#endif

private Q_SLOTS:
	void retryReply();

private:
	RestReply *q;
};

}

#endif // QTRESTCLIENT_RESTREPLY_P_H
