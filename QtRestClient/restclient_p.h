#ifndef QRESTCLIENT_P_H
#define QRESTCLIENT_P_H

#include "jsonserializer.h"
#include "restclient.h"

namespace QtRestClient {

class RestClientPrivate
{
	friend class RestClient;

public:
	static QNetworkAccessManager *getNam(RestClient *client);

private:
	QUrl baseUrl;
	QVersionNumber apiVersion;
	HeaderHash headers;
	QUrlQuery query;
	QSslConfiguration sslConfig;

	QNetworkAccessManager *nam;
	JsonSerializer *serializer;
	QScopedPointer<PagingFactory> pagingFactory;

	RestClass *rootClass;

	RestClientPrivate(RestClient *q_ptr);

	static QHash<QString, RestClient*> globalApis;
};

}

#endif // QRESTCLIENT_P_H
