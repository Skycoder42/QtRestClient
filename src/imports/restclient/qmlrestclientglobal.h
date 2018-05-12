#ifndef QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H
#define QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H

#include <QtCore/QObject>
#include <QtRestClient/qtrestclient_global.h>

#include "qmlrestclass.h"
#include "qmlpaging.h"

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtRestClient {

/*! @brief A QML singleton to access global QtRestClient functionality and to create Paging objects
 *
 * @extends QtQml.QtObject
 * @since 2.0
 */
class QtRestClient
#else
namespace QtRestClient {

class QmlRestClientGlobal : public QObject
#endif
{
	Q_OBJECT

public:
	//! @private
	explicit QmlRestClientGlobal(QJSEngine *engine, QObject *parent = nullptr);

	//! Makes the given API available under the given name
	Q_INVOKABLE bool addGlobalApi(const QString &name, QtRestClient::RestClient *client);
	//! Removes a previously added API from the global list
	Q_INVOKABLE void removeGlobalApi(const QString &name, bool deleteClient = true);
	//! Returns the client for given API name
	Q_INVOKABLE QtRestClient::RestClient *apiClient(const QString &name);
#ifdef DOXYGEN_RUN
	//! Returns the clients root class for the given API name
	Q_INVOKABLE RestClass *apiRootClass(const QString &name, QObject *parent = nullptr);
#else
	Q_INVOKABLE QtRestClient::QmlRestClass *apiRootClass(const QString &name, QObject *parent = nullptr);
#endif
#ifdef DOXYGEN_RUN
	//! Creates a new API class based on the client for the given API name
	Q_INVOKABLE RestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);
#else
	Q_INVOKABLE QtRestClient::QmlRestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);
#endif

#ifdef DOXYGEN_RUN
	//! Creates a new QML Paging object for the given client from the given object
	Q_INVOKABLE Paging createPaging(QtRestClient::RestClient *client, const QVariantMap &data);
#else
	Q_INVOKABLE QtRestClient::QmlPaging createPaging(QtRestClient::RestClient *client, const QVariantMap &data);
#endif

private:
	QJSEngine *_engine;
};

}

#endif // QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H
