#ifndef QRESTCLIENT_H
#define QRESTCLIENT_H

#include <QtCore/qglobal.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>

#if defined(QT_BUILD_RESTCLIENT_LIB)
#	define Q_RESTCLIENT_EXPORT Q_DECL_EXPORT
#else
#	define Q_RESTCLIENT_EXPORT Q_DECL_IMPORT
#endif

//! The Namespace containing all classes of the QtRestClient module
namespace QtRestClient {

class RestClient;
class RestClass;

typedef QHash<QByteArray, QByteArray> HeaderHash;

bool addGlobalApi(const QString &name, RestClient *client);
void removeGlobalApi(const QString &name, bool deleteClient = true);
RestClient *apiClient(const QString &name);
RestClass *apiRootClass(const QString &name);
RestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);
}

#endif // QRESTCLIENT_H
