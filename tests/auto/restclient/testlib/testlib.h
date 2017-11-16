#ifndef TESTLIB_H
#define TESTLIB_H

#include <QtRestClient>
#include <QtTest>
#include "jphpost.h"
#include "httpserver.h"

class Testlib
{
public:
	static void initTestJsonServer(HttpServer *server, QString relativeDbPath = QStringLiteral("default-test-db.js"));
	static QtRestClient::RestClient *createClient(QObject *parent = nullptr);
};

Q_DECLARE_METATYPE(QUrlQuery)
Q_DECLARE_METATYPE(QNetworkRequest::Attribute)

#endif // TESTLIB_H
