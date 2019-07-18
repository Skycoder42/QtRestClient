#ifndef QTRESTCLIENTAUTH_AUTHRESTCLIENT_H
#define QTRESTCLIENTAUTH_AUTHRESTCLIENT_H

#include <QtRestClient/restclient.h>

#include "QtRestClientAuth/qtrestclientauth_global.h"
#include "QtRestClientAuth/authrequestbuilder.h"

namespace QtRestClient {

class AuthRestClientPrivate;
//! An extension of the RestClient that uses a AuthRequestBuilder to create authenticated requests
class Q_RESTCLIENTAUTH_EXPORT AuthRestClient : public RestClient
{
	Q_OBJECT

public:
	//! Constructor with the OAuth instance to use for authenticating requests
	explicit AuthRestClient(QAbstractOAuth *oAuth, QObject *parent = nullptr);

	//! Returns the used OAuth instance
	QAbstractOAuth *oAuth() const;

	//! Returns the same as builder(), but as AuthRequestBuilder instance
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
