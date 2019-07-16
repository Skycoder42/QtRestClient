#ifndef QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_P_H
#define QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_P_H

#include "authrequestbuilder.h"

#include <QtRestClient/private/requestbuilder_p.h>

namespace QtRestClient {

struct AuthRequestBuilderPrivate : public RequestBuilderPrivate
{
	QAbstractOAuth *oAuth;

	AuthRequestBuilderPrivate(const QUrl &baseUrl, QAbstractOAuth *oAuth, QNetworkAccessManager *nam);
	AuthRequestBuilderPrivate(const AuthRequestBuilderPrivate &other) = default;

	void prepareRequest(QNetworkRequest &request, QByteArray *sBody) const override;
};

}

#endif // QTRESTCLIENTAUTH_AUTHREQUESTBUILDER_P_H
