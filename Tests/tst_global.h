#ifndef TST_GLOBAL_H
#define TST_GLOBAL_H

#include <QtRestClient>
#include <QtTest>
#include "jphpost.h"

Q_DECLARE_METATYPE(QUrlQuery)
Q_DECLARE_METATYPE(QNetworkRequest::Attribute)

void initTestJsonServer(QString relativeDbPath = "./default-test-db.js");

#endif // TST_GLOBAL_H
