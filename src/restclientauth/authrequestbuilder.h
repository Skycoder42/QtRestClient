#ifndef QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_H
#define QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_H

#include "QtRestClientAuth/qtrestclientauth_global.h"

#include <QtCore/QScopedPointer>

#include <QtNetworkAuth/qabstractoauth.h>

#include <QtRestClient/requestbuilder.h>

namespace QtRestClient {

class AuthExtenderPrivate;
class Q_RESTCLIENTAUTH_EXPORT AuthExtender : public RequestBuilder::IExtender
{
public:
	AuthExtender(QAbstractOAuth *oAuth);
	~AuthExtender() override;

	bool requiresBody() const override;
	void extendRequest(QNetworkRequest &request, QByteArray &verb, QByteArray *body) const override;

private:
	QScopedPointer<AuthExtenderPrivate> d;
};

//! An extension of the RequestBuilder that uses a QAbstractOAuth to create authenticated requests
class Q_RESTCLIENTAUTH_EXPORT AuthRequestBuilder : public RequestBuilder
{
public:
	//! Constructs a builder with the given base url and an OAuth instance to authenticate requests with
	AuthRequestBuilder(const QUrl &baseUrl, QAbstractOAuth *oAuth = nullptr, QNetworkAccessManager *nam = nullptr);
	//! Copy constructor
	AuthRequestBuilder(const AuthRequestBuilder &other);
	//! Move constructor
	AuthRequestBuilder(AuthRequestBuilder &&other) noexcept;
	//! Copy assignment operator
	AuthRequestBuilder &operator=(const AuthRequestBuilder &other);
	//! Move assignment operator
	AuthRequestBuilder &operator=(AuthRequestBuilder &&other) noexcept;
	~AuthRequestBuilder();

	//! Sets the OAUth instance to authenticate requests created via build() or send()
	AuthRequestBuilder &setOAuth(QAbstractOAuth *oAuth, bool replaceNam = true);

private:
	friend class AuthRestClient;
	AuthRequestBuilder(RequestBuilder &&extendedBase);
};

}

#endif // QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_H
