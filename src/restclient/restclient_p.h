#ifndef QRESTCLIENT_P_H
#define QRESTCLIENT_P_H

#include <QtJsonSerializer/QJsonSerializer>
#include "restclient.h"

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestClientPrivate
{
	friend class RestClient;

public:
	static QHash<QString, RestClient*> globalApis;

	QUrl baseUrl;
	QVersionNumber apiVersion;
	HeaderHash headers;
	QUrlQuery query;
	QHash<QNetworkRequest::Attribute, QVariant> attribs;
	QSslConfiguration sslConfig;

	QNetworkAccessManager *nam;
	QJsonSerializer *serializer;
	QScopedPointer<PagingFactory> pagingFactory;

	RestClass *rootClass;

	RestClientPrivate(RestClient *q_ptr);
};

}

#endif // QRESTCLIENT_P_H
