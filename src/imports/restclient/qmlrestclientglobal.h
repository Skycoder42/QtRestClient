#ifndef QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H
#define QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H

#include <QtCore/QObject>
#include <QtRestClient/qtrestclient_global.h>

#include "qmlrestclass.h"
#include "qmlpaging.h"

namespace QtRestClient {

class QmlRestClientGlobal : public QObject
{
	Q_OBJECT
public:
	explicit QmlRestClientGlobal(QJSEngine *engine, QObject *parent = nullptr);

	Q_INVOKABLE bool addGlobalApi(const QString &name, QtRestClient::RestClient *client);
	Q_INVOKABLE void removeGlobalApi(const QString &name, bool deleteClient = true);
	Q_INVOKABLE QtRestClient::RestClient *apiClient(const QString &name);
	Q_INVOKABLE QtRestClient::QmlRestClass *apiRootClass(const QString &name, QObject *parent = nullptr);
	Q_INVOKABLE QtRestClient::QmlRestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);

	Q_INVOKABLE QtRestClient::QmlPaging createPaging(QtRestClient::RestClient *client, const QVariantMap &data);

private:
	QJSEngine *_engine;
};

}

#endif // QTRESTCLIENT_QMLRESTCLIENTGLOBAL_H
