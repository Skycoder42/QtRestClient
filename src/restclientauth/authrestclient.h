#ifndef QTRESTCLIENTAUTH_AUTHRESTCLIENT_H
#define QTRESTCLIENTAUTH_AUTHRESTCLIENT_H

#include <QtRestClient/restclient.h>

#include "QtRestClientAuth/qtrestclientauth_global.h"
#include "QtRestClientAuth/authrequestbuilder.h"

namespace QtRestClient::Auth {

class AuthRestClientPrivate;
//! An extension of the RestClient that uses a AuthRequestBuilder to create authenticated requests
class Q_RESTCLIENTAUTH_EXPORT AuthRestClient : public RestClient
{
	Q_OBJECT

public:
	//! Constructor with the OAuth instance to use for authenticating requests
	explicit AuthRestClient(QAbstractOAuth *oAuth, QObject *parent = nullptr);
	//! Constructor with the data mode and an OAuth instance to use for authenticating requests
	explicit AuthRestClient(DataMode dataMode, QAbstractOAuth *oAuth, QObject *parent = nullptr);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Constructor with a serializer and an OAuth instance to use for authenticating requests
	explicit AuthRestClient(QtJsonSerializer::SerializerBase *serializer, QAbstractOAuth *oAuth, QObject *parent = nullptr);
#endif

	//! Returns the used OAuth instance
	QAbstractOAuth *oAuth() const;

	//! Returns the same as builder(), but as AuthRequestBuilder instance
	AuthRequestBuilder authBuilder() const;
	RequestBuilder builder() const override;

protected:
	//! @private
	AuthRestClient(AuthRestClientPrivate &dd, QObject *parent);
	//! @private
	void setupOAuth(QAbstractOAuth *oAuth);

private:
	Q_DECLARE_PRIVATE(AuthRestClient)
};

}

#endif // QTRESTCLIENTAUTH_AUTHRESTCLIENT_H
