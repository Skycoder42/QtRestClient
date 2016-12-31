#ifndef QRESTCLIENT_P_H
#define QRESTCLIENT_P_H

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

	RestClientPrivate(RestClient *q_ptr);
};

}

#endif // QRESTCLIENT_P_H
