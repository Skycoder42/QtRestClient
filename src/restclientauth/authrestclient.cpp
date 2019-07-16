#include "authrestclient.h"
#include "authrestclient_p.h"
#include "authrequestbuilder.h"
#include <QtRestClient/private/restclient_p.h>
using namespace QtRestClient;

AuthRestClient::AuthRestClient(QAbstractOAuth *oAuth, QObject *parent) :
	AuthRestClient{parent, new AuthRestClientPrivate{}}
{
	d_ptr()->oAuth = oAuth;
	oAuth->setParent(this);
	d_ptr()->nam = oAuth->networkAccessManager();
	d_ptr()->nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

QAbstractOAuth *AuthRestClient::oAuth() const
{
	return d_ptr()->oAuth;
}

AuthRequestBuilder AuthRestClient::authBuilder() const
{
	AuthRequestBuilder builder{d_ptr()->baseUrl, d_ptr()->oAuth, d_ptr()->nam};
	d_ptr()->setupBuilder(builder);
	return builder;
}

RequestBuilder AuthRestClient::builder() const
{
	// is OK here, because the actual oAuth implementation is part of the private part
	return authBuilder();
}

AuthRestClient::AuthRestClient(QObject *parent, AuthRestClientPrivate *d_ptr) :
	RestClient{parent, d_ptr, true}
{}

AuthRestClientPrivate *AuthRestClient::d_ptr()
{
	return static_cast<AuthRestClientPrivate*>(RestClient::d_ptr());
}

const AuthRestClientPrivate *AuthRestClient::d_ptr() const
{
	return static_cast<const AuthRestClientPrivate*>(RestClient::d_ptr());
}
