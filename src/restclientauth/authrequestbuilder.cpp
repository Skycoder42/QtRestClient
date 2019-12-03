#include "authrequestbuilder.h"
#include <QtCore/QPointer>
using namespace QtRestClient;

namespace QtRestClient {

class AuthExtenderPrivate
{
public:
	QPointer<QAbstractOAuth> oAuth;
};

}

AuthExtender::AuthExtender(QAbstractOAuth *oAuth) :
	  d{new AuthExtenderPrivate{}}
{
	d->oAuth = oAuth;
}

AuthExtender::~AuthExtender() = default;

bool AuthExtender::requiresBody() const
{
	return true;
}

void AuthExtender::extendRequest(QNetworkRequest &request, QByteArray &verb, QByteArray *body) const
{
	Q_ASSERT(body);
	d->oAuth->prepareRequest(&request, verb, *body);
}



AuthRequestBuilder::AuthRequestBuilder(const QUrl &baseUrl, QAbstractOAuth *oAuth, QNetworkAccessManager *nam) :
	RequestBuilder{baseUrl, nam}
{
	if (oAuth)
		setOAuth(oAuth, !nam);
}

AuthRequestBuilder &AuthRequestBuilder::setOAuth(QAbstractOAuth *oAuth, bool replaceNam)
{
	setExtender(new AuthExtender{oAuth});
	if (replaceNam)
		setNetworkAccessManager(oAuth->networkAccessManager());
	return *this;
}

AuthRequestBuilder::AuthRequestBuilder(const AuthRequestBuilder &other) = default;

AuthRequestBuilder::AuthRequestBuilder(AuthRequestBuilder &&other) noexcept = default;

AuthRequestBuilder &AuthRequestBuilder::operator=(const AuthRequestBuilder &other) = default;

AuthRequestBuilder &AuthRequestBuilder::operator=(AuthRequestBuilder &&other) noexcept = default;

AuthRequestBuilder::~AuthRequestBuilder() = default;

AuthRequestBuilder::AuthRequestBuilder(RequestBuilder &&extendedBase) :
	  RequestBuilder{std::move(extendedBase)}
{}
