#include "qtrestclient_plugin.h"

#include <QtQml>

#include <QtRestClient/RestReply>
#include <QtRestClient/PagingModel>

#include "qmlrestclass.h"
#include "qmlrestclient.h"
#include "qmlpaging.h"
#include "qmlrestclientglobal.h"
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include "qmlgenericrestreply.h"
#endif

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

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	qRegisterMetaType<QtRestClient::QmlGenericRestReply*>();
#endif
	qRegisterMetaType<QtRestClient::QmlPaging>();

	//Version 2.0
	qmlRegisterUncreatableType<QtRestClient::RestReply>(uri, 2, 0, "RestReply", QStringLiteral("RestReplies can only be returned from restclass methods"));
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	qmlRegisterUncreatableType<QtRestClient::QmlGenericRestReply>(uri, 2, 0, "GenericRestReply", QStringLiteral("GenericRestReplies can only be returned from generated restclass methods"));
#endif
	qmlRegisterUncreatableType<QtRestClient::QmlPaging>(uri, 2, 0, "Paging", QStringLiteral("Pagings can only be returned from the QtRestClient singleton"));

	qmlRegisterType<QtRestClient::QmlRestClient>(uri, 2, 0, "RestClient");
	qmlRegisterType<QtRestClient::QmlRestClass>(uri, 2, 0, "RestClass");

	qmlRegisterSingletonType<QtRestClient::QmlRestClientGlobal>(uri, 2, 0, "QtRestClient", create_qtrestclient);

	//Version 2.1
	qmlRegisterModule(uri, 2, 1);

	//Version 2.2
	qmlRegisterType<QtRestClient::PagingModel>(uri, 2, 2, "PagingModel");

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 2 && VERSION_MINOR == 2, "QML module version needs to be updated");
}
