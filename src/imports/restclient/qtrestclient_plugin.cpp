#include "qtrestclient_plugin.h"

#include <QtQml>

#include <QtRestClient/RestReply>

#include "qmlrestclass.h"
#include "qmlrestclient.h"

QtRestClientDeclarativeModule::QtRestClientDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{}

void QtRestClientDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.RestClient") == 0);

	//Version 1.3
	qmlRegisterUncreatableType<QtRestClient::RestReply>(uri, 1, 3, "RestReply", QStringLiteral("RestReplies can only be returned from restclass methods"));
	qmlRegisterType<QtRestClient::QmlRestClient>(uri, 1, 3, "RestClient");
	qmlRegisterType<QtRestClient::QmlRestClass>(uri, 1, 3, "RestClass");

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 3, "QML module version needs to be updated");
}
