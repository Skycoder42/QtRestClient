#include "qtrestclient_plugin.h"

#include <QtQml>

#include <QtRestClient/RestReply>

#include "qmlrestclass.h"
#include "qmlrestclient.h"
#include "qmlpaging.h"
#include "qmlrestclientglobal.h"

namespace {

QObject *create_qtrestclient(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	return new QtRestClient::QmlRestClientGlobal(jsEngine, qmlEngine);
}

}

QtRestClientDeclarativeModule::QtRestClientDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{}

void QtRestClientDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.RestClient") == 0);

	//Version 1.3
	qmlRegisterUncreatableType<QtRestClient::RestReply>(uri, 1, 3, "RestReply", QStringLiteral("RestReplies can only be returned from restclass methods"));
	qmlRegisterUncreatableType<QtRestClient::QmlPaging>(uri, 1, 3, "Paging", QStringLiteral("Pagings can only be returned from the QtRestClient singleton"));

	qmlRegisterType<QtRestClient::QmlRestClient>(uri, 1, 3, "RestClient");
	qmlRegisterType<QtRestClient::QmlRestClass>(uri, 1, 3, "RestClass");

	qmlRegisterSingletonType<QtRestClient::QmlRestClientGlobal>(uri, 1, 3, "QtRestClient", create_qtrestclient);

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 3, "QML module version needs to be updated");
}
