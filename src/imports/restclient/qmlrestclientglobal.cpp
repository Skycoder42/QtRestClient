#include "qmlrestclientglobal.h"
using namespace QtRestClient;

QmlRestClientGlobal::QmlRestClientGlobal(QJSEngine *engine, QObject *parent) :
	QObject(parent),
	_engine(engine)
{}

bool QmlRestClientGlobal::addGlobalApi(const QString &name, RestClient *client)
{
	return QtRestClient::addGlobalApi(name, client);
}

void QmlRestClientGlobal::removeGlobalApi(const QString &name, bool deleteClient)
{
	QtRestClient::removeGlobalApi(name, deleteClient);
}

RestClient *QmlRestClientGlobal::apiClient(const QString &name)
{
	return QtRestClient::apiClient(name);
}

QmlRestClass *QmlRestClientGlobal::apiRootClass(const QString &name, QObject *parent)
{
	auto api = QtRestClient::apiClient(name);
	if(!api)
		return nullptr;

	auto qClass = new QmlRestClass(api);
	qClass->classBegin();
	qClass->componentComplete();
	qClass->setParent(parent);
	return qClass;
}

QmlRestClass *QmlRestClientGlobal::createApiClass(const QString &name, const QString &path, QObject *parent)
{
	auto api = QtRestClient::apiClient(name);
	if(!api)
		return nullptr;

	auto qClass = new QmlRestClass(api);
	qClass->classBegin();
	qClass->setPath(path);
	qClass->componentComplete();
	qClass->setParent(parent);
	return qClass;
}

QmlPaging QmlRestClientGlobal::createPaging(RestClient *client, const QVariantMap &data)
{
	return QmlPaging::create(client, _engine, QJsonObject::fromVariantMap(data));
}
