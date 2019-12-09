#include "authrestclient.h"
#include "authrestclient_p.h"
#include "authrequestbuilder.h"
#include <QtRestClient/private/restclient_p.h>
using namespace QtRestClient;
using namespace QtRestClient::Auth;

AuthRestClient::AuthRestClient(QAbstractOAuth *oAuth, QObject *parent) :
	  AuthRestClient{DataMode::Json, oAuth, parent}
{}

AuthRestClient::AuthRestClient(RestClient::DataMode dataMode, QAbstractOAuth *oAuth, QObject *parent) :
	  AuthRestClient{*new AuthRestClientPrivate{}, parent}
{
	setupOAuth(oAuth);
	setDataMode(dataMode);
}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
AuthRestClient::AuthRestClient(QtJsonSerializer::SerializerBase *serializer, QAbstractOAuth *oAuth, QObject *parent) :
	  AuthRestClient{*new AuthRestClientPrivate{}, parent}
{
	setupOAuth(oAuth);
	setSerializer(serializer);
}
#endif

QAbstractOAuth *AuthRestClient::oAuth() const
{
	Q_D(const AuthRestClient);
	return d->oAuth;
}

AuthRequestBuilder AuthRestClient::authBuilder() const
{
	return AuthRequestBuilder{builder()};
}

RequestBuilder AuthRestClient::builder() const
{
	Q_D(const AuthRestClient);
	return RestClient::builder()
		.setExtender(new AuthExtender{d->oAuth});
}

AuthRestClient::AuthRestClient(AuthRestClientPrivate &dd, QObject *parent) :
	RestClient{dd, parent}
{}

void AuthRestClient::setupOAuth(QAbstractOAuth *oAuth)
{
	Q_D(AuthRestClient);
	d->oAuth = oAuth;
	d->oAuth->setParent(this);
	d->nam = oAuth->networkAccessManager();
	d->nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}
