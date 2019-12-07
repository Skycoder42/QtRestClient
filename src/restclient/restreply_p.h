#ifndef QTRESTCLIENT_RESTREPLY_P_H
#define QTRESTCLIENT_RESTREPLY_P_H

#include "restreply.h"

#include <QtCore/QPointer>

#include <QtCore/private/qobject_p.h>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestReplyPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(RestReply)
public:
	using DataType = RestReply::DataType;
	using Error = RestReply::Error;

	static const QByteArray PropertyBuffer;

	static QNetworkReply *compatSend(QNetworkAccessManager *nam, const QNetworkRequest &request, const QByteArray &verb, const QByteArray &body);

	QPointer<QNetworkReply> networkReply;
	bool autoDelete = true;
	bool allowEmptyReplies = false;
	std::chrono::milliseconds retryDelay {-1};

	void connectReply();

	void _q_replyFinished();
	void _q_retryReply();
#ifndef QT_NO_SSL
	void _q_handleSslErrors(const QList<QSslError> &errors);
#endif
};

Q_DECLARE_LOGGING_CATEGORY(logReply)

}

#endif // QTRESTCLIENT_RESTREPLY_P_H
