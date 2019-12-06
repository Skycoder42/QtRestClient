#include "qtrestclient_plugin.h"

#include <QtQml>

#include <QtRestClient/RestReply>
#include <QtRestClient/PagingModel>

#include "qmlrestreply.h"
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

	//Version 3.0
	qmlRegisterUncreatableType<QtRestClient::QmlRestReply>(uri, 3, 0, "RestReply", QStringLiteral("RestReplies can only be returned from restclass methods"));
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	qmlRegisterUncreatableType<QtRestClient::QmlGenericRestReply>(uri, 3, 0, "GenericRestReply", QStringLiteral("GenericRestReplies can only be returned from generated restclass methods"));
#endif
	qmlRegisterUncreatableType<QtRestClient::QmlPaging>(uri, 3, 0, "Paging", QStringLiteral("Pagings can only be returned from the QtRestClient singleton"));

	qmlRegisterType<QtRestClient::QmlRestClient>(uri, 3, 0, "RestClient");
	qmlRegisterType<QtRestClient::QmlRestClass>(uri, 3, 0, "RestClass");
	qmlRegisterType<QtRestClient::PagingModel>(uri, 3, 0, "PagingModel");

	qmlRegisterSingletonType<QtRestClient::QmlRestClientGlobal>(uri, 3, 0, "QtRestClient", create_qtrestclient);

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 3 && VERSION_MINOR == 0, "QML module version needs to be updated");
}
