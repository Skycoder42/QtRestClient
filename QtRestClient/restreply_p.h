#ifndef RESTREPLY_P_H
#define RESTREPLY_P_H

#include "restreply.h"

namespace QtRestClient {

class RestReplyPrivate : public QObject
{
	Q_OBJECT

public:
	QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> networkReply;
	bool autoDelete;
	bool retry;

	RestReplyPrivate(QNetworkReply *networkReply, RestReply *q_ptr);

public slots:
	void replyFinished();
	void handleSslErrors(const QList<QSslError> &errors);

private:
	RestReply *q_ptr;
};

}

#endif // RESTREPLY_P_H
