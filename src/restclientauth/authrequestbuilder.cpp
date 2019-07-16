#include "authrequestbuilder.h"
#include "authrequestbuilder_p.h"
using namespace QtRestClient;

#define authD (static_cast<AuthRequestBuilderPrivate*>(d_ptr()))
#define cAuthD (static_cast<const AuthRequestBuilderPrivate*>(d_ptr()))

AuthRequestBuilder::AuthRequestBuilder(const QUrl &baseUrl, QAbstractOAuth *oauth, QNetworkAccessManager *nam) :
	RequestBuilder{new AuthRequestBuilderPrivate{baseUrl, oauth, nam ? nam : (oauth ? oauth->networkAccessManager() : nullptr)}}
{}

AuthRequestBuilder &AuthRequestBuilder::setOAuth(QAbstractOAuth *oAuth, bool replaceNam)
{
	authD->oAuth = oAuth;
	if (replaceNam)
		authD->nam = oAuth->networkAccessManager();
	return *this;
}

AuthRequestBuilder::AuthRequestBuilder(const AuthRequestBuilder &other) = default;

AuthRequestBuilder::AuthRequestBuilder(AuthRequestBuilder &&other) noexcept = default;

AuthRequestBuilder &AuthRequestBuilder::operator=(const AuthRequestBuilder &other) = default;

AuthRequestBuilder &AuthRequestBuilder::operator=(AuthRequestBuilder &&other) noexcept = default;

AuthRequestBuilder::~AuthRequestBuilder() = default;

// ------------- Private Implementation -------------

QtRestClient::AuthRequestBuilderPrivate::AuthRequestBuilderPrivate(const QUrl &baseUrl, QAbstractOAuth *oAuth, QNetworkAccessManager *nam) :
	RequestBuilderPrivate{baseUrl, nam},
				   oAuth{oAuth}
{}

void AuthRequestBuilderPrivate::prepareRequest(QNetworkRequest &request, QByteArray *sBody) const
{
	QByteArray xBody;
	if (!sBody)
		sBody = &xBody;

	RequestBuilderPrivate::prepareRequest(request, sBody);
	oAuth->prepareRequest(&request, verb, *sBody);
}
