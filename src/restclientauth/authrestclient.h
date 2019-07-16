#ifndef QTRESTCLIENTAUTH_AUTHRESTCLIENT_H
#define QTRESTCLIENTAUTH_AUTHRESTCLIENT_H

#include <QtRestClient/restclient.h>

#include "QtRestClientAuth/qtrestclientauth_global.h"
#include "QtRestClientAuth/authrequestbuilder.h"

namespace QtRestClient {

class AuthRestClientPrivate;
class Q_RESTCLIENTAUTH_EXPORT AuthRestClient : public RestClient
{
	Q_OBJECT

public:
	explicit AuthRestClient(QAbstractOAuth *oAuth, QObject *parent = nullptr);

	QAbstractOAuth *oAuth() const;

	AuthRequestBuilder authBuilder() const;
	RequestBuilder builder() const override;

protected:
	//! @private
	AuthRestClient(QObject *parent, AuthRestClientPrivate *d_ptr);
	//! @private
	AuthRestClientPrivate *d_ptr();
	//! @private
	const AuthRestClientPrivate *d_ptr() const;
};

}

#endif // QTRESTCLIENTAUTH_AUTHRESTCLIENT_H
