#ifndef QTRESTCLIENTAUTH_AUTHRESTCLIENT_P_H
#define QTRESTCLIENTAUTH_AUTHRESTCLIENT_P_H

#include "authrestclient.h"

#include <QtRestClient/private/restclient_p.h>

namespace QtRestClient {

class AuthRestClientPrivate : public RestClientPrivate
{
public:
	QAbstractOAuth *oAuth = nullptr;
};

}

#endif // QTRESTCLIENTAUTH_AUTHRESTCLIENT_P_H
