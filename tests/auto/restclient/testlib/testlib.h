#ifndef TESTLIB_H
#define TESTLIB_H

#include <QtRestClient>
#include <QtTest>
#include "jphpost.h"
#include "httpserver.h"

class Testlib
{
public:
	static QtRestClient::RestClient *createClient(QObject *parent = nullptr);
};

Q_DECLARE_METATYPE(QUrlQuery)
Q_DECLARE_METATYPE(QNetworkRequest::Attribute)

#endif // TESTLIB_H
