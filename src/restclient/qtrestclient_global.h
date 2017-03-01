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

//! A typedef for a collection of HTTP-Request headers
typedef QHash<QByteArray, QByteArray> HeaderHash;

//! Makes the given API available under the given name
bool addGlobalApi(const QString &name, RestClient *client);
//! Removes a previously added API from the global list
void removeGlobalApi(const QString &name, bool deleteClient = true);
//! Returns the client for given API name
RestClient *apiClient(const QString &name);
//! Returns the clients root class for the given API name
RestClass *apiRootClass(const QString &name);
//! Creates a new API class based on the client for the given API name
RestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);
}

#endif // QRESTCLIENT_H
